#pragma once
#include "LTVector.h"
#include "LTRotation.h"
#include "LTransform.h"

struct IntersectInfo
{
	D3DXVECTOR3 vImpactPos;
	char spacer00[32];
	__int64 hObjImpact;
	unsigned long unk1;
	unsigned long unk2;
	unsigned long unk3;
};
class IntersectQuery {
public:
	IntersectQuery() {
		m_Flags = 0;
		m_FilterFn = 0;
		m_PolyFilterFn = 0;
		m_FilterActualIntersectFn = 0;
		m_pActualIntersectUserData = NULL;
		m_pUserData = NULL;
	}
	D3DXVECTOR3 m_From;
	D3DXVECTOR3 m_To;
	__int64 m_Flags;
	void* m_FilterFn;
	__int64 m_FilterActualIntersectFn;
	__int64 m_PolyFilterFn;
	void* m_pUserData;
	void* m_pActualIntersectUserData;
};

enum class GradientOrientation
{
	Horizontal,
	Vertical
};

struct FOV
{
	POINT ScreenCenter;
	LONG Distance;
	FOV() {
		ScreenCenter = { 0, 0 };
		Distance = { 0 };
	}
	FOV(POINT a, LONG b) {
		ScreenCenter = a;
		Distance = b;
	}
	bool Inited() {
		return ScreenCenter.x != 0 && ScreenCenter.y != 0;
	}
};

struct AimSearchResult {
	UINT PlayerIndex;
	//LTransform BoneTransform;
	D3DXVECTOR3 BonePos;
	UINT FramesCount;
	UINT FramesCountNeeded;
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

typedef unsigned int LTRESULT;
typedef void* LTObject;
typedef __int64(__fastcall* tIntersectSegment)(const IntersectQuery& iQuery, IntersectInfo* pInfo);
typedef signed __int64(__fastcall* tSetObjectDims)(void*, void*, LTVector<float>*, unsigned __int32);
typedef char(__fastcall* tGetNodeTransform)(void*, void*, __int64, LTransform*, bool);
typedef __int64(__fastcall* tDoScreenshot)(__int64 a);
