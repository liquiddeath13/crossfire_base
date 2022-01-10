#pragma once

enum class PlayersType {
	Opponents,
	Teammates,
	All
};

template <typename T>
std::vector<T>& operator +=(std::vector<T>& vector1, const std::vector<T>& vector2) {
	vector1.insert(vector1.end(), vector2.begin(), vector2.end());
	return vector1;
}

struct CacheSystem {
private:
	PlayersManager* PlayersMgr = new PlayersManager();
	std::pair<std::vector<Player*>, size_t> Opponents, Teammates;
	Player* Myself;
	AimSearchResult AimbotTarget;
	AimSearchType SearchType;
public:
	CacheSystem(): Myself(nullptr), 
				   Opponents(std::make_pair(std::vector<Player*>(16, nullptr), 0)),
				   Teammates(std::make_pair(std::vector<Player*>(16, nullptr), 0)),
				   AimbotTarget(),
				   SearchType((AimSearchType)Settings->GetInt(xc("AimSearchType")))
	{

	}
	bool Ready() {
		auto Local = PlayersMgr->Local();
		if (Local != nullptr) {
			return Local->IsValid();
		}
		return false;
	}
	Player* GetMyself() {
		return Myself;
	}
	AimSearchResult GetAimTarget() {
		return AimbotTarget;
	}
	void Update(LPDIRECT3DDEVICE9 pDev) {
		auto LocalPlayer = PlayersMgr->Local();
		AimSearchResult TargetSearchResult{ -1,{} };
		DOUBLE closestPos = 99999;
		if (LocalPlayer != nullptr && LocalPlayer->IsValid()) {
			for (size_t i = 0; i < 16; i++)
			{
				auto player = PlayersMgr->GetPlayerByIndex(i);
				if (player != nullptr && player->IsValid()) {
					
					player->Local = player->ClientID == LocalPlayer->ClientID;

					if (player->Local) {
						Myself = player;
					}

					player->TeammateToLocal = !player->IsOpponentTo(LocalPlayer->TeamIndex);

					if ((!player->TeammateToLocal || player->Local) && player->IsAlive() && player->ExistOnScreen(pDev)) {
						auto qv = player->IsVisibleByOpponents(player->Local ? Opponents : Teammates, LocalPlayer, 16, pDev);
						player->VisibleBy = qv.first;
						player->VisibleBoneID = qv.second;
						player->VisibleByLocalPlayer = player->VisibleBy == VISIBLE_BY::Local;
						if (!player->Local && Settings->GetBool(xc("Aimbot")) && GI->AimFOV.Inited() && player->VisibleBoneID != -1 && player->VisibleBy == VISIBLE_BY::Local) {
							//auto boneTransform = player->GetBoneTransform(boneIndex);
							auto boneIdx = player->BoneVisibleInFOV(pDev, LocalPlayer, -1, true, GI->AimFOV);
							if (boneIdx != -1) {
								auto bonePos = player->GetBoneTransform2D(boneIdx, pDev);
								bool should = false;
								auto distance = GetDistance(LocalPlayer->GetBoneTransform(6).m_Pos, player->GetBoneTransform(6).m_Pos);
								auto aimDistance = GetDistance({ static_cast<LONG>(bonePos.x), static_cast<LONG>(bonePos.y) }, GI->AimFOV.ScreenCenter);
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
									if (aimDistance <= GI->AimFOV.Distance && aimDistance < closestPos)
									{
										closestPos = aimDistance;
										should = true;
									}
									break;
								}
								if (should) {
									TargetSearchResult.PlayerIndex = i;
									//TargetSearchResult.BoneTransform = boneTransform;
									TargetSearchResult.BonePos = bonePos;
								}
							}
						}
					}

					if (player->TeammateToLocal) {
						Teammates.first[i] = player;
						++Teammates.second;
					}
					else {
						Opponents.first[i] = player;
						++Opponents.second;
					}

				}
				else {
					if (Opponents.first[i] != nullptr) {
						Opponents.first[i] = nullptr;
						--Opponents.second;
					}
					if (Teammates.first[i] != nullptr) {
						Teammates.first[i] = nullptr;
						--Teammates.second;
					}
				}
			}
			if (TargetSearchResult.PlayerIndex != -1) {
				AimbotTarget = TargetSearchResult;
			}
			else {
				AimbotTarget.PlayerIndex = -1;
			}
		}
	}
	std::vector<Player*> GetPlayers(PlayersType type) {
		std::vector<Player*> v;
		switch (type) {
		case PlayersType::Opponents:
			v = Opponents.first;
			break;
		case PlayersType::Teammates:
			v = Teammates.first;
			break;
		case PlayersType::All:
			v += Opponents.first;
			v += Teammates.first;
			break;
		}
		return v;
	}
};

CacheSystem* Cache;

//void UpdateCacheRoutine() {
//
//	while (true) {
//
//		Cache->Update();
//
//		Sleep(120);
//
//	}
//
//}