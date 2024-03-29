#pragma once

//struct IntersectInfo
//{
//	D3DXVECTOR3 vImpactPos;
//	char spacer00[32];
//	__int64 hObjImpact;
//	unsigned long unk1;
//	unsigned long unk2;
//	unsigned long unk3;
//};
//class IntersectQuery {
//public:
//	IntersectQuery() {
//		m_Flags = 0;
//		m_FilterFn = 0;
//		m_PolyFilterFn = 0;
//		m_FilterActualIntersectFn = 0;
//		m_pActualIntersectUserData = NULL;
//		m_pUserData = NULL;
//	}
//	D3DXVECTOR3 m_From;
//	D3DXVECTOR3 m_To;
//	__int64 m_Flags;
//	void* m_FilterFn;
//	__int64 m_FilterActualIntersectFn;
//	__int64 m_PolyFilterFn;
//	void* m_pUserData;
//	void* m_pActualIntersectUserData;
//};

enum class GradientOrientation
{
	Horizontal,
	Vertical
};

struct FOV
{
	LONG InitialDistance;
	LONG RescaledDistance;
	FOV() {
		InitialDistance = { 0 };
	}
	FOV(LONG a) {
		InitialDistance = a;
		RescaledDistance = a;
	}
	bool Inited() {
		return InitialDistance != 0;
	}
	void RescaleByDistance(float actual_distance, float max_delta_distance, float min_delta_distance, float factor) {
		RescaledDistance = InitialDistance;
		if (actual_distance > max_delta_distance) {
			size_t timesLower = round(actual_distance / max_delta_distance);
			if (timesLower > 0) {
				auto decrease = factor * timesLower;
				RescaledDistance *= decrease > 1 ? 0 : (1 - decrease);
			}
		}
		if (actual_distance < min_delta_distance) {
			size_t timesBigger = round(min_delta_distance / actual_distance);
			if (timesBigger > 0) {
				auto increase = factor * timesBigger;
				RescaledDistance *= (1 + increase);
			}
		}
	}
};

struct TSResult {
	UINT PlayerIndex;
	UINT BoneId;
	HOBJECT PlayerObject;
	//LTransform BoneTransform;
	std::pair<POINT, LTVector> BonePos;
	UINT FramesCount;
	UINT FramesCountNeeded;
	float DistanceThroughMap;
};

enum class AimSearchType {
	ClosestToCrosshair,
	ClosestByDistance,
	ByLowestHealth
};

typedef HRESULT(__stdcall* PresentFn)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef HRESULT(__stdcall* ResetFn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(__stdcall* tReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(__stdcall* tPresent)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef HRESULT(__stdcall* tEndScene)(LPDIRECT3DDEVICE9);

typedef LTRESULT(__fastcall* tIntersectSegment)(const IntersectQuery& iQuery, IntersectInfo* pInfo);
typedef LTRESULT(__fastcall* tSetObjectDims)(void*, void*, LTVector*, unsigned __int32);
typedef char(__fastcall* tGetNodeTransform)(void*, void*, __int64, LTransform*, bool);
typedef LTRESULT(__fastcall* tDoScreenshot)(__int64 a);
typedef LTRESULT(__fastcall* tSend2Server)(void*, ILTMessage_Read*, uint32);