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
		IntersectQuery pQuery;
		IntersectInfo pInfo;
		pQuery.m_From = FromPos3D;
		pQuery.m_To = ToPos3D;
		return !((tIntersectSegment)addresses[xc("IntersectSegment")])(pQuery, &pInfo);
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
		return GetDistance(GetBoneTransform(6).m_Pos, rval->GetBoneTransform(6).m_Pos);
	}
	LTransform GetBoneTransform(UINT Bone) {
		LTransform Trans;
		if (addresses[xc("GetNodeTransform")]) ((tGetNodeTransform)addresses[xc("GetNodeTransform")])(0, Object, Bone, &Trans, true);
		return Trans;
	}
	D3DXVECTOR3 GetBoneTransform2D(UINT Bone, IDirect3DDevice9* pDev) {
		auto bonePos = GetBoneTransform(Bone).m_Pos.ToDX();
		if (WorldToScreen(pDev, &bonePos)) {
			return bonePos;
		}
		return { 0,0,0 };
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
	UINT IsVisible(Player* From, UINT targetBone = -1, UINT fromBone = 6, bool withPriorityList = true) {
		auto fromHeadPos = From->GetBoneTransform(fromBone).m_Pos;
		if (targetBone != -1) {
			return IsVisible(fromHeadPos, GetBoneTransform(targetBone).m_Pos) ? targetBone : -1;
		}
		if (withPriorityList) {
			for (size_t i = 6; i > 0; i--)
			{
				if (IsVisible(fromHeadPos, GetBoneTransform(i).m_Pos)) {
					return i;
				}
			}
		}
		for (UINT i = 7; i < 30; i++)
		{
			if (IsVisible(fromHeadPos, GetBoneTransform(i).m_Pos)) {
				return i;
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