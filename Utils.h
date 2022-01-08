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