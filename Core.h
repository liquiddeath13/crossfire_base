#pragma once

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDev, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
	//OBS CAN'T CAPTURE THAT IF CHECKED "DON'T CAPTURE EXTERNAL OVERLAYS"

	auto presentStart = high_resolution_clock::now();
	D3D->SetDevice(pDev);
	//D3D->DrawTxt(20, 20, std::string(xc("present hook called ") + std::to_string(++presentHookCalledTimes) + xc(" times")).c_str(), Gold);

	if (Settings->GetBool(xc("DrawFeaturesStatus"))) {
		GI->SetDebugLine("Aimbot", xc("state: ") + Settings->GetBool(xc("Aimbot")) ? xc("ON") : xc("OFF"));
		GI->SetDebugLine("ESP", xc("state: ") + Settings->GetBool(xc("ESP")) ? xc("ON") : xc("OFF"));
		GI->SetDebugLine("SilentAim", xc("state: ") + Settings->GetBool(xc("SilentAim")) ? xc("ON") : xc("OFF"));
		GI->SetDebugLine("TriggerBot", xc("state: ") + Settings->GetBool(xc("TriggerBot")) ? xc("ON") : xc("OFF"));
	}
	
	if (Settings->GetBool(xc("DrawDebug"))) {
		D3D->DrawDebug();

		if (Settings->GetBool(xc("DrawRadius"))) {
			if (Settings->GetBool(xc("Aimbot"))) {
				D3D->DrawCircle(GI->ScreenCenter.x, GI->ScreenCenter.y, GI->AimbotFOV.InitialDistance, Blue);
			}
			if (Settings->GetBool(xc("TriggerBot"))) {
				D3D->DrawCircle(GI->ScreenCenter.x, GI->ScreenCenter.y, GI->TriggerBotFOV.RescaledDistance, Green);
			}
			if (Settings->GetBool(xc("SilentAim"))) {
				D3D->DrawCircle(GI->ScreenCenter.x, GI->ScreenCenter.y, GI->SilentAimFOV.InitialDistance, White);
			}
		}
	}
	if (Settings->GetBool(xc("ESP"))) {
		auto LocalPlayer = PlayersMgr->Local();

		if (LocalPlayer != nullptr && LocalPlayer->IsValid() && LocalPlayer->IsAlive()) {


			if (Settings->GetBool(xc("YouAreVisibleAlarm"))) {

				GI->ClearVisibleByList();

			}

			for (size_t i = 0; i < 16; i++)
			{
				auto player = PlayersMgr->GetPlayerByIndex(i);

				if (player != nullptr && player->IsValid() && player->IsAlive() && player->IsOpponentTo(LocalPlayer)) {

					bool visible = player->FastIsVisibleBy(LocalPlayer, 6);

					auto espColor = visible ? Green : Red;
					auto center = GetScreenCenter(D3D->GetDevice());

					if (Settings->GetBool(xc("Skeletons"))) {

						D3D->DrawSkeleton(player, LocalPlayer, espColor);

					}

					if (Settings->GetBool(xc("LinesFromCenterToHead"))) {

						auto headBone = player->GetBoneTransform(6, D3D->GetDevice());
						if (headBone.first.x != 0 && headBone.first.y != 0) {
							D3D->DrawLine(center.x, center.y, headBone.first.x, headBone.first.y, espColor);
						}

					}

					if (Settings->GetBool(xc("YouAreVisibleAlarm"))) {

						if (LocalPlayer->FastIsVisibleBy(player, 5)) {

							GI->AppendVisibleByList(player->Nickname, i);

						}

					}

				}

			}

			if (Settings->GetBool(xc("YouAreVisibleAlarm"))) {

				D3D->DrawVisibleByList();

			}

		}
	}

	if (Settings->GetBool(xc("CountPerformance"))) {
		auto presentEnd = high_resolution_clock::now();
		duration<double, std::milli> ms_double = presentEnd - presentStart;
		GI->SetDebugLine(xc("hkPresent performance"), xc("delay: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}

	return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDev, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	D3D->HandleOnLost();
	auto result = oReset(pDev, pPresentationParameters);
	D3D->HandleOnReset();
	return result;
}

TSResult GetTarget(UINT preferableBone, FOV f, AimSearchType SearchType, std::string callerFn) {
	GI->ScreenCenter = GetScreenCenter(D3D->GetDevice());
	auto LocalPlayer = PlayersMgr->Local();
	TSResult TargetSearchResult{ -1,{} };
	DOUBLE closestPos = 99999;
	if (!D3D->IsDeviceValid() || (LocalPlayer == nullptr || !LocalPlayer->IsValid() || !LocalPlayer->IsAlive()) || GI->ScreenCenter.x == 0) {
		GI->SetDebugLine(callerFn, xc("GetTarget: not valid"));
		return TargetSearchResult;
	}
	else {
		//GI->SetDebugLine(callerFn, xc("GetTarget: searching..."));
	}
	for (size_t i = 0; i < 16; i++)
	{
		auto player = PlayersMgr->GetPlayerByIndex(i);

		if (player != nullptr && player->IsValid() && player->IsAlive() && player->IsOpponentTo(LocalPlayer)) {

			if (preferableBone == -1) {
				preferableBone = player->GetVisibleBone(LocalPlayer, D3D->GetDevice(), f, true);
				if (preferableBone == -1) {
					continue;
				}
			}
			else {
				if (!player->FastIsVisibleBy(LocalPlayer, preferableBone)) {
					continue;
				}
			}

			auto bonePos = player->GetBoneTransform(preferableBone, D3D->GetDevice());

			if (bonePos.first.x != 0 && bonePos.first.y != 0) {

				auto distanceFromCrosshair = GetDistance(bonePos.first, GI->ScreenCenter);

				if (distanceFromCrosshair <= f.InitialDistance) {

					auto distance = player->DistanceTo(LocalPlayer);
					bool should = false;

					switch (SearchType) {
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
						if (distanceFromCrosshair < closestPos)
						{
							closestPos = distanceFromCrosshair;
							should = true;
						}
						break;
					}
					if (should) {
						TargetSearchResult.PlayerIndex = i;
						TargetSearchResult.BonePos = bonePos;
						TargetSearchResult.DistanceThroughMap = distance;
						TargetSearchResult.PlayerObject = player->Object;
						TargetSearchResult.BoneId = preferableBone;
					}

				}

			}

		}

	}

	if (TargetSearchResult.PlayerIndex != -1) {
		GI->SetDebugLine(
			callerFn,
			xc("GetTarget: ") + std::string(PlayersMgr->GetPlayerByIndex(TargetSearchResult.PlayerIndex)->Nickname) + xc(", ") + std::to_string(TargetSearchResult.BoneId)
		);
	}

	return TargetSearchResult;
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev) {
	auto endsceneStart = high_resolution_clock::now();
	duration<double, std::milli> ms_double;
	D3D->SetDevice(pDev);
	//D3D->DrawTxt(20, 25, std::string(xc("endscene hook called ") + std::to_string(++endsceneHookCalledTimes) + xc(" times")).c_str(), White);

	if (gaks(Settings->GetInt(xc("AimKeyONOFF"))) & 1) {
		Settings->SetBool(xc("Aimbot"), !Settings->GetBool(xc("Aimbot")));
	}
	/*if (gaks(Settings->GetInt(xc("TriggerKeyONOFF"))) & 1) {
		Settings->SetBool(xc("TriggerBot"), !Settings->GetBool(xc("TriggerBot")));
	}
	if (gaks(Settings->GetInt(xc("SilentAimONOFF"))) & 1) {
		Settings->SetBool(xc("SilentAim"), !Settings->GetBool(xc("SilentAim")));
	}
	if (gaks(VK_F5) & 1) {
		Settings->SetBool(xc("DrawDebug"), !Settings->GetBool(xc("DrawDebug")));
		Settings->SetBool(xc("DrawRadius"), !Settings->GetBool(xc("DrawRadius")));
	}*/

	++endsceneHookCalledTimes;
	auto Local = PlayersMgr->Local();
	if (Settings->GetBool(xc("Aimbot")) && Local != nullptr && Local->IsValid() && Local->IsAlive()) {
		auto t1 = high_resolution_clock::now();
		auto AimbotTarget = GetTarget(
			Settings->GetInt(xc("AimbotBone")),
			GI->AimbotFOV,
			(AimSearchType)(Settings->GetInt(xc("AimbotSearchType"))),
			xc("hkEndScene")
		);

		if (Settings->GetBool(xc("CountPerformance"))) {
			auto t2 = high_resolution_clock::now();
			ms_double = t2 - t1;
			GI->SetDebugLine(xc("hkEndScene GetTarget performance"), xc("time: ") + std::to_string(ms_double.count()) + xc(" ms"));
		}

		if (AimbotTarget.PlayerIndex != -1) {

			t1 = high_resolution_clock::now();

			{
				if (Settings->GetBool(xc("TriggerBot")) && !gaks(VK_LBUTTON)) {

					if (endsceneHookCalledTimes % 7 > 2) {
						GI->TriggerBotFOV.RescaleByDistance(AimbotTarget.DistanceThroughMap, 3000, 1400, 0.2f);

						if ((AimbotTarget.BoneId >= 3 && AimbotTarget.BoneId <= 6) || AimbotTarget.BoneId == 14) {
							GI->TriggerBotFOV.RescaledDistance *= AimbotTarget.DistanceThroughMap < 700 ? 10 : 1.5f;
						}

						if (IsBoneInFOV(AimbotTarget.BonePos.first, GI->TriggerBotFOV)) {

							if (Settings->GetBool(xc("AutoTrigger")) || (gaks(Settings->GetInt(xc("TriggerKey"))) & 0x8000)) {
								mevt(MOUSEEVENTF_LEFTDOWN, 0, 10, 0, 0);
								mevt(MOUSEEVENTF_LEFTUP, 0, 10, 0, 0);
							}

						}
					}

				}

			}

			if (Settings->GetBool(xc("CountPerformance"))) {
				auto t2 = high_resolution_clock::now();
				ms_double = t2 - t1;
				GI->SetDebugLine(xc("hkEndScene TriggerBotRoutine performance"), xc("time: ") + std::to_string(ms_double.count()) + xc(" ms"));
			}

			t1 = high_resolution_clock::now();

			{
				if (!gaks(VK_LBUTTON)) {
					if (Settings->GetBool(xc("AutoAim")) || (gaks(Settings->GetInt(xc("AimKey"))) & 0x8000)) {
						aim->Do({ AimbotTarget.BonePos.first.x - GI->ScreenCenter.x, AimbotTarget.BonePos.first.y - GI->ScreenCenter.y - 1 });
					}
				}

			}

			if (Settings->GetBool(xc("CountPerformance"))) {
				auto t2 = high_resolution_clock::now();
				ms_double = t2 - t1;
				GI->SetDebugLine(xc("hkEndScene AimbotRoutine performance"), xc(" time: ") + std::to_string(ms_double.count()) + xc(" ms"));
			}
		}
	}
	

	if (Settings->GetBool(xc("CountPerformance"))) {
		auto endsceneEnd = high_resolution_clock::now();
		ms_double = endsceneEnd - endsceneStart;
		GI->SetDebugLine(xc("hkEndscene performance"), xc("delay: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}


	return oEndScene(pDev);
}

bool InitDXHooks() {
	return D3D->HookPresent(&hkPresent) && D3D->HookReset(&hkReset) && D3D->HookEndScene(&hkEndScene);
}

bool DeinitDXHooks() {
	return D3D->HookPresent(NULL, false) && D3D->HookReset(NULL, false) && D3D->HookEndScene(NULL, false);
}

float hb_multiplier[] = { 2.5f, 2.5f, 2.5f, 2.5f };
void setDims(LTVector* dim) {
	if (*dim == defaultGoldHead) {
		dim->x *= hb_multiplier[0];
		dim->y *= hb_multiplier[0];
		dim->z *= hb_multiplier[0];
	}
	if (*dim == defaultSilverHead) {
		dim->x *= hb_multiplier[1];
		dim->y *= hb_multiplier[1];
		dim->z *= hb_multiplier[1];
	}
	if (*dim == defaultBody) {
		dim->x *= hb_multiplier[2];
		dim->y *= hb_multiplier[2];
		dim->z *= hb_multiplier[2];
	}
	if (*dim == defaultPelvis) {
		dim->x *= hb_multiplier[3];
		dim->y *= hb_multiplier[3];
		dim->z *= hb_multiplier[3];
	}
}

signed __int64 __fastcall hkSetObjectDims(void* a1, HOBJECT hObj, LTVector* dim, UINT32 flags) {
	
	if (flags == 0) {
		setDims(dim);
	}
	return ((tSetObjectDims)addresses[xc("SetObjectDims")])(a1, hObj, dim, flags);
}

bool hkIntersectSegment(IntersectQuery* Query, IntersectInfo* Info)
{
	auto intersectStart = high_resolution_clock::now();
	auto fn = ((tIntersectSegment)addresses[xc("IntersectSegment")]);
	if (Settings->GetBool(xc("SilentAim"))) {

		auto SilentAimTarget = GetTarget(
			Settings->GetInt(xc("SilentAimBone")), 
			GI->SilentAimFOV, 
			(AimSearchType)(Settings->GetInt(xc("SilentAimSearchType"))),
			xc("hkIntersectSegment")
		);

		if (SilentAimTarget.PlayerIndex != -1) {
			auto radius = LTVector(0.0f, 30.f, 0.0f);

			Query->m_From = SilentAimTarget.BonePos.second + radius;
			Query->m_To = SilentAimTarget.BonePos.second - radius;
			Query->m_Flags = ::INTERSECT_OBJECTS | ::IGNORE_NONSOLID | ::INTERSECT_HPOLY;
			Query->m_FilterFn = [](HOBJECT hObj, void* pUser) ->bool {
				for (int i = 0; i < 16; i++)
				{
					Player* player = PlayersMgr->GetPlayerByIndex(i);

					if (player != nullptr && player->IsValid() && hObj == player->Object)
					{
						return false;
					}

				}
				return true;
			};

			bool Result = fn(*Query, Info);
			if (Result)
			{
				Info->m_hObject = SilentAimTarget.PlayerObject;
				Info->m_Point = SilentAimTarget.BonePos.second;
				return fn(*Query, Info);
			}

		}
	}

	if (Settings->GetBool(xc("CountPerformance"))) {
		auto intersectEnd = high_resolution_clock::now();
		duration<double, std::milli> ms_double = intersectEnd - intersectStart;
		GI->SetDebugLine(xc("hkIntersectSegment performance"), xc("delay: ") + std::to_string(ms_double.count()) + xc(" ms"));
	}
	return fn(*Query, Info);
}