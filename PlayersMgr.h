#pragma once

enum class VISIBLE_BY {
	None,
	Me,
	Teammate,
	Opponent
};

/*
object в 0x0 занимает 8 байт
clientid в 0x8 занимает 1 байт
teamindex в 0x9 занимает 1 байт
nickname в 0xA занимает 14 байт
hasc4 в 0x28 занимает 1 байт
health в 0x44 занимает 4 байта
kills в 0x48 занимает 4 байта
deaths в 0x4C занимает 4 байта
headshots в 0x50 занимает 4 байта
ping в 0x54 занимает 4 байта
*/
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
	int32_t Ping;
	int32_t Team;

	bool operator==(Player* rval) {
		return rval->ClientID == ClientID;
	}
	bool operator!=(Player* rval) {
		return rval->ClientID != ClientID;
	}
	D3DXVECTOR3 GetPos3D() {
		return GetPtrValue<D3DXVECTOR3>((PBYTE)Object, 0x15C);
	}
	bool IsOpponentTo(char teamIndex) {
		return TeamIndex != teamIndex;
	}
	bool IsValid() {
		return Object != NULL && Health > 0;
	}
	LTransform GetBoneTransform(UINT Bone) {
		LTransform Trans;
		if (addresses[xc("GetNodeTransform")]) ((tGetNodeTransform)addresses[xc("GetNodeTransform")])(0, Object, Bone, &Trans, true);
		return Trans;
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
	bool IsVisibleBy(Player* rval, size_t targetBone = -1) {
		auto FromPos3D = GetBoneTransform(6).m_Pos;
		if (targetBone != -1) {
			auto ToPos3D = rval->GetBoneTransform(targetBone).m_Pos;
			IntersectQuery pQuery;
			IntersectInfo pInfo;
			pQuery.m_From = D3DXVECTOR3{ FromPos3D.x, FromPos3D.y, FromPos3D.z };
			pQuery.m_To = D3DXVECTOR3{ ToPos3D.x, ToPos3D.y, ToPos3D.z };
			return !((tIntersectSegment)addresses[xc("IntersectSegment")])(pQuery, &pInfo);
		}
		for (size_t i = 1; i < 30; i++)
		{
			auto ToPos3D = rval->GetBoneTransform(i).m_Pos;
			IntersectQuery pQuery;
			IntersectInfo pInfo;
			pQuery.m_From = D3DXVECTOR3{ FromPos3D.x, FromPos3D.y, FromPos3D.z };
			pQuery.m_To = D3DXVECTOR3{ ToPos3D.x, ToPos3D.y, ToPos3D.z };
			if (!((tIntersectSegment)addresses[xc("IntersectSegment")])(pQuery, &pInfo)) {
				return true;
			}
		}
		return false;
	}
	VISIBLE_BY IsVisibleByOpponents(Player** players, Player* local, size_t len) {
		if (ClientID != local->ClientID) {
			if (IsVisibleBy(local)) {
				return VISIBLE_BY::Me;
			}
		}
		if (len > 12) {
			//prevent massive fps drop
			return VISIBLE_BY::None;
		}
		for (size_t i = 0; i < len; i++)
		{
			auto player = players[i];
			if (player != nullptr && player->IsValid() && player->ClientID != local->ClientID && player->IsOpponentTo(TeamIndex)) {
				if (IsVisibleBy(player)) {
					return ClientID == local->ClientID ? VISIBLE_BY::Opponent : VISIBLE_BY::Teammate;
				}
			}
		}
		return VISIBLE_BY::None;
	}
};

struct PlayersManager {
private:
	char GetMyIndex() {
		if (!addresses[xc("pLTClientShell")]) {
			return -1;
		}
		char clients = GetPtrValue<char>((PBYTE)addresses[xc("pLTClientShell")], 0x289);
		return clients < 16 ? clients : -1;
	}
	Player* GetPlayerByIndex(int PlayerIdx) {
		if (PlayerIdx == 255 || !addresses[xc("pLTClientShell")]) {
			return NULL;
		}
		return (Player*)(addresses[xc("pLTClientShell")] + 0x290 + (PlayerIdx * 0xD98));
	}
public:
	Player* Me() {
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
	static UINT BoneVisible(Player* From, Player* To, UINT targetBone = -1, bool withPriorityList = true) {
		if (targetBone != -1) {
			return To->IsVisibleBy(From, targetBone) ? targetBone : -1;
		}
		if (withPriorityList) {
			for (auto mainBonesID : { 6, 5, 4, 3, 2, 1 })
			{
				if (To->IsVisibleBy(From, mainBonesID)) {
					return mainBonesID;
				}
			}
		}
		for (UINT i = 7; i < 30; i++)
		{
			if (To->IsVisibleBy(From, i)) {
				return i;
			}
		}
		return -1;
	}
};

PlayersManager* PlayersMgr = new PlayersManager();