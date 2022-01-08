#pragma once

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

enum THREADINFOCLASS { ThreadHideFromDebugger = 0x11 };

typedef NTSTATUS(WINAPI* NtQueryInformationThread_t)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);
typedef NTSTATUS(WINAPI* NtSetInformationThread_t)(HANDLE, THREADINFOCLASS, PVOID, ULONG);

NtQueryInformationThread_t fnNtQueryInformationThread = NULL;
NtSetInformationThread_t fnNtSetInformationThread = NULL;

int StartHidden(LPTHREAD_START_ROUTINE t) {
	DWORD dwThreadId = 0;
	HANDLE hThread = ct(NULL, 0, t, NULL, 0, &dwThreadId);

	HMODULE hDLL = GetModuleBaseAddress(xw(L"ntdll.dll"));
	if (!hDLL) return -1;

	fnNtQueryInformationThread = (NtQueryInformationThread_t)GetExportAddress(hDLL, xc("NtQueryInformationThread"));
	fnNtSetInformationThread = (NtSetInformationThread_t)GetExportAddress(hDLL, xc("NtSetInformationThread"));

	if (!fnNtQueryInformationThread || !fnNtSetInformationThread) return -1;

	ULONG lHideThread = 1, lRet = 0;

	fnNtSetInformationThread(hThread, ThreadHideFromDebugger, &lHideThread, sizeof(lHideThread));
	fnNtQueryInformationThread(hThread, ThreadHideFromDebugger, &lHideThread, sizeof(lHideThread), &lRet);

	return TRUE;
}

DWORD64* GetD3DVtable(HWND hWnd)
{
	typedef LPDIRECT3D9(__stdcall* D3DCreate)(UINT);
	HMODULE hDLL = GetModuleBaseAddress(xw(L"d3d9"));
	if (hDLL == nullptr) { return nullptr; }
	D3DCreate pDirect3DCreate9 = (D3DCreate)GetExportAddress(hDLL, xc("Direct3DCreate9"));

	LPDIRECT3D9 pD3D = pDirect3DCreate9(D3D_SDK_VERSION);
	D3DDISPLAYMODE d3ddm;
	HRESULT hRes = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	IDirect3DDevice9* ppReturnedDeviceInterface = nullptr;

	hRes = pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		d3dpp.hDeviceWindow,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &ppReturnedDeviceInterface
	);

	DWORD64* d3dVtable = NULL;

	if (ppReturnedDeviceInterface != nullptr) {
		d3dVtable = (DWORD64*)(*((DWORD64*)ppReturnedDeviceInterface));
	}

	pD3D->Release();
	return d3dVtable;
}

bool IsGameReadyForHook() {
	return GetModuleBaseAddress(xw(L"CShell_x64.dll")) && GetModuleBaseAddress(xw(L"ClientFx_x64.fxd"));
}