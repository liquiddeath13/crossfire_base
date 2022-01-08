#pragma once

struct Player {
	void* Object;
	const char* Nickname;
	char TeamIndex;
	char ClientID;
	short Health;
	short Kills;
	int32_t Deaths;
	int32_t Headshots;
	int32_t Ping;
	bool HasC4;
	bool Local;
	D3DXVECTOR3 GetObjectPos() {
		return GetPtrValue<D3DXVECTOR3>((PBYTE)Object, 0x15C);
	}
	bool operator==(Player rval) {
		return rval.ClientID == ClientID;
	}
	bool operator!=(Player rval) {
		return rval.ClientID != ClientID;
	}
	bool IsOpponent(char teamIndex) {
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
};

