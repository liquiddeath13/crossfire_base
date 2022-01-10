#pragma once

enum class VISIBLE_BY {
	None,
	Local,
	Teammate,
	Opponent
};

struct Player {
	void* Object;
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
	bool VisibleByLocalPlayer;
	bool Local;
	bool TeammateToLocal;
	VISIBLE_BY VisibleBy;
	UINT VisibleBoneID;

	bool operator==(Player* rval) {
		return rval->ClientID == ClientID;
	}
	bool operator!=(Player* rval) {
		return rval->ClientID != ClientID;
	}
	D3DXVECTOR3 GetPos3D() {
		return GetPtrValue<D3DXVECTOR3>((PBYTE)Object, 0x15C);
	}
	LTVector<float> GetPosLT() {
		auto v = GetPos3D();
		return {v.x, v.y, v.z};
	}
	bool IsOpponentTo(char teamIndex) {
		return TeamIndex != teamIndex;
	}
	bool IsValid() {
		return Object != NULL;
	}
	bool IsAlive() {
		return Health > 0;
	}
	bool ExistOnScreen(LPDIRECT3DDEVICE9 pDev) {
		return GetBoneTransform2D(6, pDev).x != 0;
	}
	LTransform GetBoneTransform(UINT Bone) {
		LTransform Trans;
		if (addresses[xc("GetNodeTransform")]) ((tGetNodeTransform)addresses[xc("GetNodeTransform")])(0, Object, Bone, &Trans, true);
		return Trans;
	}
	D3DXVECTOR3 GetBoneTransform2D(UINT Bone, LPDIRECT3DDEVICE9 pDev) {
		auto bone3d = GetBoneTransform(Bone).m_Pos.ToDX();
		if (WorldToScreen(pDev, &bone3d)) {
			bone3d.y -= 2.f;
			return bone3d;
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
	std::pair<bool, UINT> IsVisibleBy(Player* rval, size_t targetBone = -1) {
		auto FromPos3D = GetBoneTransform(5).m_Pos;
		if (targetBone != -1) {
			auto ToPos3D = rval->GetBoneTransform(targetBone).m_Pos;
			IntersectQuery pQuery;
			IntersectInfo pInfo;
			pQuery.m_From = D3DXVECTOR3{ FromPos3D.x, FromPos3D.y, FromPos3D.z };
			pQuery.m_To = D3DXVECTOR3{ ToPos3D.x, ToPos3D.y, ToPos3D.z };
			auto qv = !((tIntersectSegment)addresses[xc("IntersectSegment")])(pQuery, &pInfo);
			return std::make_pair(qv, qv ? targetBone : -1);
		}
		for (size_t i = 1; i < 30; i++)
		{
			auto qv = IsVisibleBy(rval, i);
			if (qv.first) {
				return std::make_pair(true, i);
			}
		}
		return std::make_pair(false, -1);
	}
	std::pair<VISIBLE_BY, UINT> IsVisibleByOpponents(std::pair<std::vector<Player*>, size_t> players, Player* local, size_t len, LPDIRECT3DDEVICE9 pDev) {
		if (ClientID != local->ClientID) {
			auto q = IsVisibleBy(local);
			if (q.first) {
				return std::make_pair(VISIBLE_BY::Local, q.second);
			}
		}
		if (players.second < 2 || !Settings->GetBool(xc("GhostESP"))) {
			return std::make_pair(VISIBLE_BY::None, -1);
		}
		if (Settings->GetBool(xc("PreventFpsDrop"))) {
			if (players.second > Settings->GetInt(xc("DisableGhostESPAtPlayersCount"))) {
				return std::make_pair(VISIBLE_BY::None, -1);
			}
		}
		for (size_t i = 0; i < len; i++)
		{
			auto player = players.first[i];
			if (
				player != nullptr 
				&& player->IsValid()
				&& player->ExistOnScreen(pDev)
				&& player->ClientID != local->ClientID
				&& player->IsOpponentTo(TeamIndex)
				&& !player->IsOpponentTo(local->TeamIndex)
			) {
				auto q = IsVisibleBy(player);
				if (q.first) {
					return ClientID == local->ClientID ? std::make_pair(VISIBLE_BY::Opponent, q.second) : std::make_pair(VISIBLE_BY::Teammate, q.second);
				}
			}
		}
		return std::make_pair(VISIBLE_BY::None, -1);
	}
	bool BoneInFOV(LPDIRECT3DDEVICE9 pDev, UINT targetBone, FOV f) {
		return IsBoneInFOV(GetBoneTransform2D(targetBone, pDev), f);
	}
	UINT BoneVisibleInFOV(LPDIRECT3DDEVICE9 pDev, Player* From, UINT targetBone, bool withPriorityList, FOV f) {
		if (targetBone != -1) {
			bool boneVisible = IsVisibleBy(From, targetBone).first && BoneInFOV(pDev, targetBone, f) ? targetBone : -1;
		}
		if (withPriorityList) {
			for (auto mainBonesID : { 6, 5, 4, 3, 2, 1 })
			{
				if (IsVisibleBy(From, mainBonesID).first && BoneInFOV(pDev, mainBonesID, f)) {
					return mainBonesID;
				}
			}
		}
		for (UINT i = 7; i < 30; i++)
		{
			if (IsVisibleBy(From, i).first && BoneInFOV(pDev, i, f)) {
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
	std::pair<Player**, size_t> RetrieveAll() {
		size_t playerCount = 16;
		Player** list = new Player*[playerCount];
		for (size_t i = 0; i < playerCount; i++)
		{
			list[i] = GetPlayerByIndex(i);
		}
		return std::make_pair(list, playerCount);
	}
};