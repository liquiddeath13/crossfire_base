#pragma once

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

struct Core {
private:

public:
	Core() {

	}
	/*static void __stdcall StartCacheThread(void* Param)
	{
		MyClass* This = (MyClass*)Param;
		return This->ThreadStart();
	}*/
	void AimbotRoutine() {
		if (Settings->GetBool(xc("Aimbot"))) {

			if (Cache->Ready() && D3D->IsDeviceValid()) {
				
				auto AimFOV = GI->AimFOV;
				auto TriggerBotFOV = GI->TriggerBotFOV;

				if (AimFOV.Inited() && TriggerBotFOV.Inited()) {

					if (Settings->GetBool(xc("DrawRadius"))) {
						D3D->DrawCircle(AimFOV.ScreenCenter.x, AimFOV.ScreenCenter.y, GetAimbotCircleRadius(), Blue);
						D3D->DrawCircle(TriggerBotFOV.ScreenCenter.x, TriggerBotFOV.ScreenCenter.y, GetTriggerBotCircleRadius(), Green);
					}

					auto target = Cache->GetAimTarget();

					if (target.PlayerIndex != -1) {

						D3DXVECTOR3 bonePos = target.BonePos;

						auto dx = static_cast<LONG>(bonePos.x - GI->AimFOV.ScreenCenter.x);
						auto dy = static_cast<LONG>(bonePos.y - GI->AimFOV.ScreenCenter.y);

						if (Settings->GetBool(xc("AutoAim")) || (gaks(Settings->GetInt(xc("AimKey"))) & 0x8000)) {
							aim->Do({ dx, dy });
						}

						if (IsBoneInFOV(bonePos, TriggerBotFOV)) {

							if (Settings->GetBool(xc("AutoTrigger")) || (gaks(Settings->GetInt(xc("TriggerKey"))) & 0x8000)) {
								mevt(MOUSEEVENTF_LEFTDOWN, 0, 6, 0, 0);
								mevt(MOUSEEVENTF_LEFTUP, 0, 6, 0, 0);
							}

						}

					}

				}

			}

		}
	}
	void DrawESP() {
		if (Settings->GetBool(xc("ESP"))) {

			if (Cache->Ready() && D3D->IsDeviceValid()) {

				auto opponents = Cache->GetPlayers(PlayersType::Opponents);

				for (size_t i = 0; i < 16; i++)
				{
					auto player = opponents[i];

					if (player != nullptr && player->IsValid() && player->ExistOnScreen(D3D->GetDevice()) && player->IsAlive()) {

						D3DCOLOR color;

						switch (player->VisibleBy) {

						case VISIBLE_BY::Local:
							color = D3DCOLOR_COLORVALUE(0.f, 1.f, 0.f, 1.f);
							break;

						case VISIBLE_BY::Teammate:
							color = D3DCOLOR_COLORVALUE(1.f, 1.f, 1.f, 1.f);
							break;

						default:
							color = D3DCOLOR_COLORVALUE(1.f, 0.f, 0.f, 1.f);
							break;

						}

						if (Settings->GetBool(xc("Skeletons"))) {
							D3D->DrawSkeleton(player, Cache->GetMyself(), color);
						}

						/*if (Settings->GetBool(xc("2DBoxes"))) {
							D3D->DrawBox2D(player, color);
						}*/

						if (Settings->GetBool(xc("LinesFromCenterToHead"))) {

							auto center = GetScreenCenter(D3D->GetDevice());
							auto headBone = player->GetBoneTransform2D(6, D3D->GetDevice());
							D3D->DrawLine(center.x, center.y, headBone.x, headBone.y, color);

						}

					}
				}

			}
		}
	}
	void VisibleAlarm() {
		if (Settings->GetBool(xc("YouAreVisibleAlarm"))) {

			if (Cache && Cache->Ready() && D3D->IsDeviceValid()) {

				auto center = GetScreenCenter(D3D->GetDevice());

				auto visibleStr = std::string(xc("YOU'RE VISIBLE BY OPPONENT"));

				switch (Cache->GetMyself()->VisibleBy) {

				case VISIBLE_BY::Opponent:
					D3D->DrawTxt(center.x - visibleStr.length() / 2, center.y - center.y * .2f, visibleStr.c_str(), Red);
					break;

				}

			}

		}
	}
};

Core* KayaCore = new Core();

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDev, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
	//OBS CAN'T CAPTURE THAT IF CHECKED "DON'T CAPTURE EXTERNAL OVERLAYS"
	if (D3D->ShouldHide()) {
		return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	D3D->SetDevice(pDev);
	//D3D->DrawTxt(20, 20, std::string(xc("present hook called ") + std::to_string(++presentHookCalledTimes) + xc(" times")).c_str(), Gold);
	auto t1 = high_resolution_clock::now();
	KayaCore->DrawESP();
	auto t2 = high_resolution_clock::now();
	duration<double, std::milli> ms_double = t2 - t1;
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(xc("execution time of DrawESP: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}
	t1 = high_resolution_clock::now();
	KayaCore->VisibleAlarm();
	t2 = high_resolution_clock::now();ms_double = t2 - t1;
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(xc("execution time of VisibleAlarm: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}
	return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDev, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	D3D->HandleOnLost();
	auto result = oReset(pDev, pPresentationParameters);
	D3D->HandleOnReset();
	return result;
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev) {
	if (D3D->ShouldHide()) {
		return oEndScene(pDev);
	}
	D3D->SetDevice(pDev);
	//D3D->DrawTxt(20, 25, std::string(xc("endscene hook called ") + std::to_string(++endsceneHookCalledTimes) + xc(" times")).c_str(), White);
	auto t1 = high_resolution_clock::now();
	Cache->Update(pDev);
	auto t2 = high_resolution_clock::now();
	duration<double, std::milli> ms_double = t2 - t1;
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(xc("execution time of Update: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}
	GI->AimFOV = { GetScreenCenter(pDev), GetAimbotCircleRadius() };
	GI->TriggerBotFOV = { GetScreenCenter(pDev), GetTriggerBotCircleRadius() };
	t1 = high_resolution_clock::now();
	KayaCore->AimbotRoutine();
	t2 = high_resolution_clock::now();
	ms_double = t2 - t1;
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(xc("execution time of AimbotRoutine: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}
	return oEndScene(pDev);
}

bool InitDXHooks() {
	return D3D->HookPresent(&hkPresent) && D3D->HookReset(&hkReset) && D3D->HookEndScene(&hkEndScene);
}

bool DeinitDXHooks() {
	return D3D->HookPresent(NULL, false) && D3D->HookReset(NULL, false) && D3D->HookEndScene(NULL, false);
}