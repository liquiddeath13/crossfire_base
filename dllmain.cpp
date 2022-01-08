// dllmain.cpp : Определяет точку входа для приложения DLL.
#include <Windows.h>
#include <Psapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <unordered_map>
#include "XorStr.h"
#include "OwnInternals.h"
#include "CallsObfuscate.h"
#include "DbgConsole.h"
#include "Utils.h"
#include "Types.h"
#include "Patterns.h"
#include "Constants.h"
#include "PlayersMgr.h"
#include "detoursx64/detours.h"
#include "Hook/hook.h"
#include "D3D.h"

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detoursx64/detours.lib")

void MainThread() {
    while (!IsGameReadyForHook()) {
        sleep(100);
    }
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Initializing MH..."));
    }
    if (MH_Initialize() == MH_OK) {
        if (DebugConsole->IsAttached()) {
            DebugConsole->PrintMsg(xc("Success!"));
        }
    }
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Initializing DX hooks..."));
    }
    if (InitDXHooks()) {
        if (DebugConsole->IsAttached()) {
            DebugConsole->PrintMsg(xc("Success!"));
        }
    }
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Initializing addresses..."));
    }
    InitAddresses();
    if (addresses[xc("pLTClientShell")]) {
        if (DebugConsole->IsAttached()) {
            DebugConsole->PrintMsg(xc("Success!"));
        }
    }
}

void Unhook() {
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Deinitializing dx hooks..."));
    }
    DeinitDXHooks();
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Success!"));
    }
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Deinitializing MH..."));
    }
    MH_Uninitialize();
    if (DebugConsole->IsAttached()) {
        DebugConsole->PrintMsg(xc("Success!"));
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    dtlc(hModule);
    if (!gmha(xc("crossfire.exe"))) return TRUE;
    RemovePeHeader(hModule);
    UnlinkModuleFromPEB(hModule);
    if (!DebugConsole->IsAttached()) {
        DebugConsole->Attach();
    }
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        StartHidden((LPTHREAD_START_ROUTINE)MainThread);
    }
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        Unhook();
        if (DebugConsole->IsAttached()) {
            DebugConsole->Detach();
        }
    }
    return TRUE;
}

