#pragma once

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDev, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
	//OBS CAN'T CAPTURE THAT IF CHECKED "DON'T CAPTURE EXTERNAL OVERLAYS"
	if (D3D->ShouldHide()) {
		return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	D3D->SetDevice(pDev);
	//D3D->DrawTxt(20, 20, std::string(xc("present hook called ") + std::to_string(++presentHookCalledTimes) + xc(" times")).c_str(), Gold);
	auto LocalPlayer = PlayersMgr->Local();

	if (LocalPlayer != nullptr && LocalPlayer->IsValid() && LocalPlayer->IsAlive()) {

		auto t1 = high_resolution_clock::now();

		for (size_t i = 0; i < 16; i++)
		{
			auto player = PlayersMgr->GetPlayerByIndex(i);

			if (player != nullptr && player->IsValid() && player->IsAlive() && player->IsOpponentTo(LocalPlayer)) {
				
				bool visible = player->IsVisible(LocalPlayer, 6) != -1;
				
				if (Settings->GetBool(xc("ESP"))) {

					auto espColor = visible ? Green : Red;
					auto center = GetScreenCenter(D3D->GetDevice());

					if (Settings->GetBool(xc("Skeletons"))) {

						D3D->DrawSkeleton(player, LocalPlayer, espColor);

					}

					if (Settings->GetBool(xc("LinesFromCenterToHead"))) {

						auto headBone = player->GetBoneTransform2D(6, D3D->GetDevice());
						if (headBone.x != 0 && headBone.z != 0) {
							D3D->DrawLine(center.x, center.y, headBone.x, headBone.y, espColor);
						}

					}

					if (Settings->GetBool(xc("YouAreVisibleAlarm"))) {

						if (LocalPlayer->IsVisible(player, 6) != -1) {
							auto visibleStr = std::string(xc("YOU'RE VISIBLE BY ") + std::string(player->Nickname));

							D3D->DrawTxt(center.x - visibleStr.length() / 1.5, center.y - center.y * .2f, visibleStr.c_str(), Red);
						}

					}

				}

			}

		}

		auto t2 = high_resolution_clock::now();
		duration<double, std::milli> ms_double = t2 - t1;
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("[hkPresent] execution time of enumeration: ") + std::to_string(ms_double.count()) + xc(" ms"));
		}
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

	if (gaks(Settings->GetInt(xc("AimKeyONOFF")))) {
		Settings->SetBool(xc("Aimbot"), !Settings->GetBool(xc("Aimbot")));
	}
	if (gaks(Settings->GetInt(xc("TriggerKeyONOFF")))) {
		Settings->SetBool(xc("TriggerBot"), !Settings->GetBool(xc("TriggerBot")));
	}

	auto LocalPlayer = PlayersMgr->Local();
	++endsceneHookCalledTimes;
	if (LocalPlayer != nullptr && LocalPlayer->IsValid() && LocalPlayer->IsAlive()) {
		
		GI->AimFOV = { GetScreenCenter(pDev), Settings->GetInt(xc("AimbotRadius")) };
		GI->TriggerBotFOV = { GetScreenCenter(pDev), Settings->GetInt(xc("TriggerBotRadius")) };
		AimSearchResult TargetSearchResult{ -1,{} };
		DOUBLE closestPos = 99999;

		auto t1 = high_resolution_clock::now();

		for (size_t i = 0; i < 16; i++)
		{
			auto player = PlayersMgr->GetPlayerByIndex(i);

			if (player != nullptr && player->IsValid() && player->IsAlive() && player->IsOpponentTo(LocalPlayer)) {
				
				if (Settings->GetBool(xc("Aimbot"))) {

					bool visible = player->IsVisible(LocalPlayer, 6) != -1;

					if (visible) {

						//head is visible
						auto bonePos = player->GetBoneTransform2D(6, D3D->GetDevice());

						if (bonePos.x != 0 && bonePos.y != 0) {

							auto aimDistance = GetDistance({ static_cast<LONG>(bonePos.x), static_cast<LONG>(bonePos.y) }, GI->AimFOV.ScreenCenter);

							if (aimDistance <= GI->AimFOV.Distance) {

								auto distance = player->DistanceTo(LocalPlayer);
								bool should = false;

								switch ((AimSearchType)(Settings->GetInt(xc("AimSearchType")))) {
								case AimSearchType::ByLowestHealth:
									if (player->Health < closestPos) {
										closestPos = player->Health;
										should = true;
									}
									break;
								case AimSearchType::ClosestByDistance:
									if (distance < closestPos) {
										closestPos = distance;
										should = true;
									}
									break;
								case AimSearchType::ClosestToCrosshair:
									if (aimDistance < closestPos)
									{
										closestPos = aimDistance;
										should = true;
									}
									break;
								}
								if (should) {
									TargetSearchResult.PlayerIndex = i;
									TargetSearchResult.BonePos = bonePos;
									TargetSearchResult.DistanceThroughMap = distance;
								}

							}

						}

					}

				}

			}

		}



		auto t2 = high_resolution_clock::now();
		duration<double, std::milli> ms_double = t2 - t1;
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("[hkEndScene] execution time of enumeration: ") + std::to_string(ms_double.count()) + xc(" ms"));
		}

		if (TargetSearchResult.PlayerIndex != -1) {
			t1 = high_resolution_clock::now();
			
			{
				if (Settings->GetBool(xc("DrawRadius"))) {
					D3D->DrawCircle(GI->AimFOV.ScreenCenter.x, GI->AimFOV.ScreenCenter.y, Settings->GetInt(xc("AimbotRadius")), Blue);
				}

				auto dx = static_cast<LONG>(TargetSearchResult.BonePos.x - GI->AimFOV.ScreenCenter.x);
				auto dy = static_cast<LONG>(TargetSearchResult.BonePos.y - GI->AimFOV.ScreenCenter.y) + 1;

				if (Settings->GetBool(xc("AutoAim")) || (gaks(Settings->GetInt(xc("AimKey"))) & 0x8000)) {
					aim->Do({ dx, dy });
				}
			}

			t2 = high_resolution_clock::now();
			ms_double = t2 - t1;
			if (DebugConsole->IsAttached()) {
				DebugConsole->PrintMsg(xc("execution time of AimbotRoutine: ") + std::to_string(ms_double.count()) + xc(" ms"));
			}

			if (endsceneHookCalledTimes % 2 == 0) {
				t1 = high_resolution_clock::now();
				
				{
					FOV f = GI->TriggerBotFOV;

					f.RescaleByDistance(TargetSearchResult.DistanceThroughMap, 3000, 600, 0.24f);

					if (Settings->GetBool(xc("DrawRadius"))) {
						D3D->DrawCircle(f.ScreenCenter.x, f.ScreenCenter.y, Settings->GetInt(xc("TriggerBotRadius")), Green);
					}

					if (IsBoneInFOV(TargetSearchResult.BonePos, f)) {

						if (Settings->GetBool(xc("AutoTrigger")) || (gaks(Settings->GetInt(xc("TriggerKey"))) & 0x8000)) {
							mevt(MOUSEEVENTF_LEFTDOWN, 0, 10, 0, 0);
							mevt(MOUSEEVENTF_LEFTUP, 0, 10, 0, 0);
						}

					}
				}

				t2 = high_resolution_clock::now();
				ms_double = t2 - t1;
				if (DebugConsole->IsAttached()) {
					DebugConsole->PrintMsg(xc("execution time of TriggerBotRoutine: ") + std::to_string(ms_double.count()) + xc(" ms"));
				}
			}
		}

	}
	return oEndScene(pDev);
}

bool InitDXHooks() {
	return D3D->HookPresent(&hkPresent) && D3D->HookReset(&hkReset) && D3D->HookEndScene(&hkEndScene);
}

bool DeinitDXHooks() {
	return D3D->HookPresent(NULL, false) && D3D->HookReset(NULL, false) && D3D->HookEndScene(NULL, false);
}