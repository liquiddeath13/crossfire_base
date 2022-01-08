#pragma once

#define readpeb __readgsqword(0x60)

HMODULE WINAPI GetModuleBaseAddress(LPCWSTR moduleName)
{
	PEB* pPeb = NULL;
	LIST_ENTRY* pListEntry = NULL;
	LDR_DATA_TABLE_ENTRY* pLdrDataTableEntry = NULL;

	pPeb = (PPEB)readpeb;

	if (pPeb == NULL)
		return NULL;

	pLdrDataTableEntry = (PLDR_DATA_TABLE_ENTRY)pPeb->Ldr->InMemoryOrderModuleList.Flink;
	pListEntry = pPeb->Ldr->InMemoryOrderModuleList.Flink;

	do
	{
		if (lstrcmpiW(pLdrDataTableEntry->FullDllName.Buffer, moduleName) == 0)
			return (HMODULE)pLdrDataTableEntry->Reserved2[0];

		pListEntry = pListEntry->Flink;
		pLdrDataTableEntry = (PLDR_DATA_TABLE_ENTRY)(pListEntry->Flink);

	} while (pListEntry != pPeb->Ldr->InMemoryOrderModuleList.Flink);

	return NULL;
}

FARPROC WINAPI GetExportAddress(HMODULE hMod, const char* lpProcName)
{
	char* pBaseAddress = (char*)hMod;

	IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pBaseAddress;
	IMAGE_NT_HEADERS* pNtHeaders = (IMAGE_NT_HEADERS*)(pBaseAddress + pDosHeader->e_lfanew);
	IMAGE_OPTIONAL_HEADER* pOptionalHeader = &pNtHeaders->OptionalHeader;
	IMAGE_DATA_DIRECTORY* pDataDirectory = (IMAGE_DATA_DIRECTORY*)(&pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	IMAGE_EXPORT_DIRECTORY* pExportDirectory = (IMAGE_EXPORT_DIRECTORY*)(pBaseAddress + pDataDirectory->VirtualAddress);

	void** ppFunctions = (void**)(pBaseAddress + pExportDirectory->AddressOfFunctions);
	WORD* pOrdinals = (WORD*)(pBaseAddress + pExportDirectory->AddressOfNameOrdinals);
	ULONG* pNames = (ULONG*)(pBaseAddress + pExportDirectory->AddressOfNames);
	/* char **pNames = (char **)(pBaseAddress + pExportDirectory->AddressOfNames); /* */

	FARPROC pAddress = NULL;

	typedef HMODULE(WINAPI* LoadLibraryAF)(LPCSTR lpFileName);
	LoadLibraryAF pLoadLibraryA = NULL;

	DWORD i;

	if (((DWORD_PTR)lpProcName >> 16) == 0)
	{
		WORD ordinal = LOWORD(lpProcName);
		DWORD dwOrdinalBase = pExportDirectory->Base;

		if (ordinal < dwOrdinalBase || ordinal >= dwOrdinalBase + pExportDirectory->NumberOfFunctions)
			return NULL;

		pAddress = (FARPROC)(pBaseAddress + (DWORD_PTR)ppFunctions[ordinal - dwOrdinalBase]);
	}
	else
	{
		for (i = 0; i < pExportDirectory->NumberOfNames; i++)
		{
			char* szName = (char*)pBaseAddress + (DWORD_PTR)pNames[i];
			if (strcmp(lpProcName, szName) == 0)
			{
				pAddress = (FARPROC)(pBaseAddress + ((ULONG*)(pBaseAddress + pExportDirectory->AddressOfFunctions))[pOrdinals[i]]);
				break;
			}
		}
	}

	if ((char*)pAddress >= (char*)pExportDirectory && (char*)pAddress < (char*)pExportDirectory + pDataDirectory->Size)
	{
		char* szDllName, * szFunctionName;
		HMODULE hForward;

		szDllName = _strdup((const char*)pAddress);
		if (!szDllName)
			return NULL;

		pAddress = NULL;
		szFunctionName = strchr(szDllName, '.');
		*szFunctionName++ = 0;

		pLoadLibraryA = (LoadLibraryAF)GetExportAddress(GetModuleBaseAddress(xw(L"KERNEL32.DLL")), xc("LoadLibraryA"));

		if (pLoadLibraryA == NULL)
			return NULL;

		hForward = pLoadLibraryA(szDllName);
		free(szDllName);

		if (!hForward)
			return NULL;

		pAddress = GetExportAddress(hForward, szFunctionName);
	}

	return pAddress;
}

HMODULE hKernel32 = GetModuleBaseAddress(xw(L"KERNEL32.DLL"));
HMODULE hUser32 = GetModuleBaseAddress(xw(L"USER32.DLL"));

#define gmha(moduleName) reinterpret_cast<decltype(&GetModuleHandleA)>(GetExportAddress(hKernel32, xc("GetModuleHandleA")))(moduleName)
#define sleep(d) reinterpret_cast<decltype(&Sleep)>(GetExportAddress(hKernel32, xc("Sleep")))(d)
#define gct reinterpret_cast<decltype(&GetCurrentThread)>(GetExportAddress(hKernel32, xc("GetCurrentThread")))()
#define dtlc(mod) reinterpret_cast<decltype(&DisableThreadLibraryCalls)>(GetExportAddress(hKernel32, xc("DisableThreadLibraryCalls")))(mod)
#define k32gmi(hp, hm, lmi, cb) reinterpret_cast<decltype(&K32GetModuleInformation)>(GetExportAddress(hKernel32, xc("K32GetModuleInformation")))(hp, hm, lmi, cb)
#define ct(attr, dss, lsa, lp, dwcc, lti) reinterpret_cast<decltype(&CreateThread)>(GetExportAddress(hKernel32, xc("CreateThread")))(attr, dss, lsa, lp, dwcc, lti)
#define gmfn(moduleName, fname, sz) reinterpret_cast<decltype(&GetModuleFileNameA)>(GetExportAddress(hKernel32, xc("GetModuleFileNameA")))(moduleName, fname, sz)
#define gcp reinterpret_cast<decltype(&GetCurrentProcess)>(GetExportAddress(hKernel32, xc("GetCurrentProcess")))()
#define vp(la, dwsz, fnp, lpfp) reinterpret_cast<decltype(&VirtualProtect)>(GetExportAddress(hKernel32, xc("VirtualProtect")))(la, dwsz, fnp, lpfp)
#define mevt(dwf, dx, dy, dwd, dwei) reinterpret_cast<decltype(&mouse_event)>(GetExportAddress(hUser32, xc("mouse_event")))(dwf, dx, dy, dwd, dwei)
#define gaks(vk) reinterpret_cast<decltype(&GetAsyncKeyState)>(GetExportAddress(hUser32, xc("GetAsyncKeyState")))(vk)
#define fwa(x, y) reinterpret_cast<decltype(&FindWindowA)>(GetExportAddress(hUser32, xc("FindWindowA")))(x, y)
