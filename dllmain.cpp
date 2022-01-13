// dllmain.cpp : Определяет точку входа для приложения DLL.
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <Psapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <unordered_map>
#include <ShlObj.h>
#include <ctime>
#include <chrono>
#include <shared_mutex>
#include "sdk/inc/iltmessage.h"
#include "XorStr.h"
#include "OwnInternals.h"
#include "CallsObfuscate.h"
#include "DbgConsole.h"
#include "Types.h"
#include "Settings.h"
#include "Sig.hpp"
#include "Colors.h"
#include "GameInfo.h"
#include "Utils.h"
#include "Patterns.h"
#include "Constants.h"
#include "PlayersMgr.h"
#include "WeaponMgr.h"
#include "Aimbot.h"
#include "detoursx64/detours.h"
#include "Hook/hook.h"
#include "D3D.h"
#include "Core.h"
//#include "OverlayGfx.h"
//#include "NoAccessProtection.h"

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detoursx64/detours.lib")

void MainThread() {
    //InitializeNoAccessProtection(hModule);
    while (!IsGameReadyForHook()) {
        sleep(100);
    }
    InitAddresses();
    MH_Initialize();
    InitDXHooks();

    aim->SetSmooth(Settings->GetFloat(xc("AimSmooth")));
    GI->AimbotFOV = { Settings->GetInt(xc("AimbotRadius")) };
    GI->TriggerBotFOV = { Settings->GetInt(xc("TriggerBotRadius")) };
    GI->SilentAimFOV = { Settings->GetInt(xc("SilentAimRadius")) };

    DetourTransactionBegin();
    DetourUpdateThread(gct);
    DetourAttach(&(PVOID&)addresses[xc("IntersectSegment")], &hkIntersectSegment);
    //DetourAttach(&(PVOID&)addresses[xc("SetObjectDims")], &hkSetObjectDims);
    DetourTransactionCommit();
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    dtlc(hModule);
    if (!gmha(xc("crossfire.exe"))) return TRUE;
    InitSettings();
    RemovePeHeader(hModule);
    UnlinkModuleFromPEB(hModule);
    if (Settings->GetBool(xc("UseDebugConsole"))) {
        if (!DebugConsole->IsAttached()) {
            DebugConsole->Attach();
        }
    }
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        StartHidden((LPTHREAD_START_ROUTINE)MainThread, xw(L"mrac64.dll"), 0x1337);
    }
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        Unhook();
        if (DebugConsole->IsAttached()) {
            DebugConsole->Detach();
        }
    }
    return TRUE;
}

