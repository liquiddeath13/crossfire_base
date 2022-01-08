// dllmain.cpp : Определяет точку входа для приложения DLL.
#include <Windows.h>
#include <Psapi.h>
#include "XorStr.h"
#include "OwnInternals.h"
#include "CallsObfuscate.h"
#include "Utils.h"
#include "detoursx64/detours.h"

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detoursx64/detours.lib")



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

