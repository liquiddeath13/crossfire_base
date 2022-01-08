// dllmain.cpp : Определяет точку входа для приложения DLL.
#include <Windows.h>
#include <Psapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "XorStr.h"
#include "OwnInternals.h"
#include "CallsObfuscate.h"
#include "Utils.h"
#include "Types.h"
#include "Patterns.h"
#include "Constants.h"
#include "detoursx64/detours.h"
#include "Hook/hook.h"
#include "D3D.h"

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detoursx64/detours.lib")

void MainThread() {
    MH_Initialize();
    InitDXHooks();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    dtlc(hModule);
    if (!gmha(xc("crossfire.exe"))) return TRUE;
    RemovePeHeader(hModule);
    UnlinkModuleFromPEB(hModule);
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        StartHidden((LPTHREAD_START_ROUTINE)MainThread);
    }
    return TRUE;
}

