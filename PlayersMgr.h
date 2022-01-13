#pragma once

enum class VISIBLE_BY {
	None,
	Local,
	Teammate,
	Opponent
};

struct Player {
private:
	bool IsVisible(LTVector FromPos3D, LTVector ToPos3D) {
		auto fn = ((tIntersectSegment)addresses[xc("IntersectSegment")]);
		IntersectQuery pQuery;
		IntersectInfo pInfo;
		pQuery.m_From = FromPos3D;
		pQuery.m_To = ToPos3D;
		pQuery.m_Flags = ::IGNORE_NONSOLID;
		return !fn(pQuery, &pInfo);
	}
public:
	HOBJECT Object;
	char ClientID;
	char TeamIndex;
	char Nickname[14];
	char spacer1[0x10];
	bool HasC4;
	char spacer2[0x1B];
	int32_t Health;
	int32_t Kills;
	int32_t Deaths;
	int32_t Headshots;
	bool VisibleToLocal;
	bool operator==(Player* rval) {
		return rval->ClientID == ClientID;
	}
	bool operator!=(Player* rval) {
		return rval->ClientID != ClientID;
	}
	bool IsOpponentTo(Player* rval) {
		return TeamIndex != rval->TeamIndex;
	}
	bool IsValid() {
		return Object != NULL;
	}
	bool IsAlive() {
		return Health > 0;
	}
	float DistanceTo(Player* rval) {
		return GetDistance(FastGetBonePos(6), rval->FastGetBonePos(6));
	}
	std::pair<POINT, LTVector> GetBoneTransform(UINT Bone, IDirect3DDevice9* pDev = nullptr) {
		LTransform Trans;
		if (addresses[xc("GetNodeTransform")]) ((tGetNodeTransform)addresses[xc("GetNodeTransform")])(0, Object, Bone, &Trans, true);
		auto bonePos = Trans.m_Pos;
		if (pDev == nullptr) {
			return std::make_pair(POINT{ 0, 0 }, bonePos);
		}
		auto native = bonePos;
		if (WorldToScreen(pDev, &bonePos)) {
			return std::make_pair(POINT{ static_cast<LONG>(bonePos.x), static_cast<LONG>(bonePos.y) }, native);
		}
		return std::make_pair(POINT{ 0, 0 }, native);
	}
	LTVector FastGetBonePos(UINT Bone) {
		LTransform Trans;
		if (addresses[xc("GetNodeTransform")]) ((tGetNodeTransform)addresses[xc("GetNodeTransform")])(0, Object, Bone, &Trans, true);
		return Trans.m_Pos;
	}
	void GetNodeTransform(UINT Bone, LTransform& Out) {
		LTransform Trans;
		if (addresses[xc("GetNodeTransform")]) ((tGetNodeTransform)addresses[xc("GetNodeTransform")])(0, Object, Bone, &Trans, true);
		Out = Trans;
		Out.m_Pos.y += 5;
	}
	LTransform GetNodeTransform(UINT bone) {
		LTransform r;
		GetNodeTransform(bone, r);
		return r;
	}
	bool IsVisibleBy(Player* From, UINT targetBone) {
		return IsVisible(From->GetBoneTransform(5).second, GetBoneTransform(targetBone).second);
	}
	bool FastIsVisibleBy(Player* From, UINT targetBone) {
		return IsVisible(From->FastGetBonePos(5), FastGetBonePos(6));
	}
	UINT GetVisibleBone(Player* From, IDirect3DDevice9* pDev = nullptr, FOV f = {}, bool withFovCheck = false, UINT targetBone = -1, UINT fromBone = 6) {
		auto fromHeadPos = From->GetBoneTransform(fromBone).second;

		if (targetBone != -1) {
			auto bt = GetBoneTransform(targetBone, pDev);
			if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
				return -1;
			}
			return IsVisible(fromHeadPos, bt.second) ? targetBone : -1;
		}

		if (IsVisible(fromHeadPos, GetBoneTransform(leftHandFirstBone).second)) {
			for (const auto& bone : upperBodyBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
			for (const auto& bone : leftHandBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
		}

		if (IsVisible(fromHeadPos, GetBoneTransform(rightHandFirstBone).second)) {
			for (const auto& bone : upperBodyBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
			for (const auto& bone : rightHandBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
		}

		if (IsVisible(fromHeadPos, GetBoneTransform(leftLegFirstBone).second)) {
			for (const auto& bone : upperBodyBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
			for (const auto& bone : lowerBodyBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
			for (const auto& bone : leftLegBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
		}

		if (IsVisible(fromHeadPos, GetBoneTransform(rightLegFirstBone).second)) {
			for (const auto& bone : upperBodyBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
			for (const auto& bone : lowerBodyBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
			for (const auto& bone : rightLegBones) {
				auto bt = GetBoneTransform(bone, pDev);
				if (withFovCheck && (bt.first.x == 0 || !IsPointInFOV(bt.first, f))) {
					continue;
				}
				if (IsVisible(fromHeadPos, bt.second)) {
					return bone;
				}
			}
		}

		return -1;
	}
};

struct PlayersManager {
private:
	DWORD64 PlayerListStart = 0x290;
	DWORD64 PlayerStructSize = 0xD98;
	char GetMyIndex() {
		if (!addresses[xc("pLTClientShell")]) {
			return -1;
		}
		char clients = GetPtrValue<char>((PBYTE)addresses[xc("pLTClientShell")], 0x289);
		return clients < 16 ? clients : -1;
	}
public:
	PlayersManager() {

	}
	Player* GetPlayerByIndex(int PlayerIdx) {
		if (PlayerIdx == 255 || !addresses[xc("pLTClientShell")]) {
			return NULL;
		}
		return (Player*)(addresses[xc("pLTClientShell")] + PlayerListStart + (PlayerIdx * PlayerStructSize));
	}
	Player* Local() {
		return GetPlayerByIndex(GetMyIndex());
	}
};

PlayersManager* PlayersMgr = new PlayersManager();