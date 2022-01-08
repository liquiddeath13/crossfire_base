#pragma once
#include <iomanip>
#include <sstream>

template< typename T >
std::string NumberToHex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}

DWORD64 GetModuleSize(HMODULE hModule)
{
    MODULEINFO module_info;
    memset(&module_info, 0, sizeof(module_info));
    if (k32gmi(gcp, hModule, &module_info, sizeof(module_info))) {
        return module_info.SizeOfImage;
    }
    return NULL;
}

bool DataCompare(BYTE* pData, BYTE* bSig, const char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bSig)
    {
        if (*szMask == 'x' && *pData != *bSig) return false;
    }
    return (*szMask) == NULL;
}

BYTE* FindPattern(BYTE* dwAddress, DWORD64 dwSize, PBYTE pbSig, const char* szMask)
{
    DWORD64 length = strlen(szMask);
    for (DWORD64 i = NULL; i < dwSize - length; i++)
    {
        if (DataCompare(dwAddress + i, pbSig, szMask)) return dwAddress + i;
    }
    return 0;
}

DWORD64 findAddress(LPCSTR moduleName, PBYTE Pattern, LPCSTR Mask) {
    HMODULE base = gmha(moduleName);
    DWORD64 moduleSize = GetModuleSize(base);
    return moduleSize > 0 ? (DWORD64)FindPattern((PBYTE)base, (DWORD64)base + moduleSize, Pattern, Mask) : 0;
}

std::vector<UNLINKED_MODULE> UnlinkedModules;

void RelinkModuleToPEB(HMODULE hModule)
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));

	if (it == UnlinkedModules.end())
	{
		return;
	}
	UNLINKED_MODULE m = *it;
	RELINK(m.Entry->InLoadOrderModuleList, m.RealInLoadOrderLinks);
	RELINK(m.Entry->InInitializationOrderModuleList, m.RealInInitializationOrderLinks);
	RELINK(m.Entry->InMemoryOrderModuleList, m.RealInMemoryOrderLinks);
	UnlinkedModules.erase(it);
}

void UnlinkModuleFromPEB(HMODULE hModule)
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));
	if (it != UnlinkedModules.end())
	{
		return;
	}

	PPEB pPEB = (PPEB)__readgsqword(0x60);
	PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
	PLDR_MODULE Current = NULL;
	while (CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		Current = CONTAINING_RECORD(CurrentEntry, LDR_MODULE, InLoadOrderModuleList);
		if (Current->BaseAddress == hModule)
		{
			UNLINKED_MODULE CurrentModule = { 0 };
			CurrentModule.hModule = hModule;
			CurrentModule.RealInLoadOrderLinks = Current->InLoadOrderModuleList.Blink->Flink;
			CurrentModule.RealInInitializationOrderLinks = Current->InInitializationOrderModuleList.Blink->Flink;
			CurrentModule.RealInMemoryOrderLinks = Current->InMemoryOrderModuleList.Blink->Flink;
			CurrentModule.Entry = Current;
			UnlinkedModules.push_back(CurrentModule);

			UNLINK(Current->InLoadOrderModuleList);
			UNLINK(Current->InInitializationOrderModuleList);
			UNLINK(Current->InMemoryOrderModuleList);

			break;
		}

		CurrentEntry = CurrentEntry->Flink;
	}
}


void RemovePeHeader(HANDLE GetModuleBase)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleBase;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader + (DWORD)pDosHeader->e_lfanew);

	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return;

	if (pNTHeader->FileHeader.SizeOfOptionalHeader)
	{
		DWORD Protect;
		WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;

		vp((void*)GetModuleBase, Size, PAGE_EXECUTE_READWRITE, &Protect);
		SecureZeroMemory((void*)GetModuleBase, Size);
		vp((void*)GetModuleBase, Size, Protect, &Protect);
	}
}

enum THREADINFOCLASS {
	ThreadQuerySetWin32StartAddress = 0x9,
	ThreadHideFromDebugger = 0x11
};

typedef NTSTATUS(NTAPI* NtCreateThreadEx_t)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, HANDLE, LPTHREAD_START_ROUTINE, LPVOID, ULONG, ULONG_PTR, SIZE_T, SIZE_T, LPVOID);
typedef NTSTATUS(NTAPI* NtQueryInformationThread_t)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);
typedef NTSTATUS(NTAPI* NtSetInformationThread_t)(HANDLE, THREADINFOCLASS, PVOID, ULONG);

NtCreateThreadEx_t fnNtCreateThreadEx = NULL;
NtQueryInformationThread_t fnNtQueryInformationThread = NULL;
NtSetInformationThread_t fnNtSetInformationThread = NULL;

int StartHidden(LPTHREAD_START_ROUTINE ThreadRoutine) {
	DWORD dwThreadId = 0;
	DWORD_PTR dwStartAddress = 0;
	ULONG lHideThread = 1, lRet = 0;
	BOOL bContextRet = FALSE;
	CONTEXT ctx{};
	HMODULE hNtDLL = loadlib(xc("ntdll.dll"));
	HANDLE hThread = INVALID_HANDLE_VALUE;
	if (!hNtDLL) {
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("!hNtDLL"));
		}
		return -1;
	}

	fnNtQueryInformationThread = (NtQueryInformationThread_t)GetExportAddress(hNtDLL, xc("NtQueryInformationThread"));
	fnNtSetInformationThread = (NtSetInformationThread_t)GetExportAddress(hNtDLL, xc("NtSetInformationThread"));
	fnNtCreateThreadEx = (NtCreateThreadEx_t)GetExportAddress(hNtDLL, xc("NtCreateThreadEx"));

	if (!fnNtQueryInformationThread || !fnNtSetInformationThread || !fnNtCreateThreadEx) {
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("!fnNtQueryInformationThread || !fnNtSetInformationThread || !fnNtCreateThreadEx"));
		}
		return -1;
	}
	auto aStart = GetModuleBaseAddress(xw(L"mrac_x64.dll")) + 0x1337;
	fnNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, 0, gcp, (LPTHREAD_START_ROUTINE)aStart /* Start address */, 0, 0x1 /* Suspended */, 0, 0, 0, 0);
	//hThread = ct(NULL, 0, ThreadRoutine, NULL, 0, &dwThreadId);
	ctx.ContextFlags = CONTEXT_ALL;
	bContextRet = gtc(hThread, &ctx);
	ctx.Rcx = (DWORD64)ThreadRoutine;
	bContextRet = stc(hThread, &ctx);
	auto dwResumeRet = rt(hThread);
	fnNtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(dwStartAddress), NULL);
	fnNtSetInformationThread(hThread, ThreadHideFromDebugger, &lHideThread, sizeof(lHideThread));
	fnNtQueryInformationThread(hThread, ThreadHideFromDebugger, &lHideThread, sizeof(lHideThread), &lRet);
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(xc("thread started at address: ") + NumberToHex(dwStartAddress));
	}

	return TRUE;
}

bool IsGameReadyForHook() {
	return GetModuleBaseAddress(xw(L"CShell_x64.dll")) && GetModuleBaseAddress(xw(L"ClientFx_x64.fxd"));
}

template<typename T>
T GetPtrValue(PBYTE ptr, DWORD offset)
{
	return *(T*)(ptr + offset);
}

template<typename T>
T* GetPtrValueMany(PBYTE ptr, DWORD offset, size_t length)
{
	T* val = new T[length]();
	memcpy_s(val, length, ptr + offset, length);
	return val;
}

size_t GetBoneRadius(D3DXVECTOR3 start, D3DXVECTOR3 end) {
	D3DXVECTOR3 Box = end - start;
	if (Box.y < 0) Box.y *= -1;
	return (int)Box.y * 1.5;
}

bool WorldToScreen(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3* InOut)
{
	D3DXVECTOR3 vScreen;
	D3DVIEWPORT9 viewPort = { 0 };
	D3DXMATRIX projection, view, world;

	// Get Player Position
	D3DXVECTOR3 PlayerPos(InOut->x, InOut->y, InOut->z);

	// Get Player Viewport
	pDevice->GetTransform(D3DTS_VIEW, &view);
	pDevice->GetTransform(D3DTS_PROJECTION, &projection);
	pDevice->GetTransform(D3DTS_WORLD, &world);
	pDevice->GetViewport(&viewPort);

	// Get player screen position from Viewport
	D3DXVec3Project(&vScreen, &PlayerPos, &viewPort, &projection, &view, &world);

	if (vScreen.z < 1.0f && vScreen.x > 0.0f && vScreen.y > 0.0f && vScreen.x < viewPort.Width && vScreen.y < viewPort.Height)
	{
		*InOut = { vScreen.x, vScreen.y, vScreen.z };
		return true;
	}

	return false;
}

D3DXVECTOR3 GetMidPoint(D3DXVECTOR3 V1, D3DXVECTOR3 V2)
{
	return D3DXVECTOR3((V1.x + V2.x) / 2, (V1.y + V2.y) / 2, (V1.z + V2.z) / 2);
}

POINT GetScreenCenter(IDirect3DDevice9* pDev)
{
	D3DVIEWPORT9 viewport;
	pDev->GetViewport(&viewport);
	return{ static_cast<LONG>(viewport.Width / 2), static_cast<LONG>(viewport.Height / 2) };
}