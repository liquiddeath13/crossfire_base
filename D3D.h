#pragma once

tPresent oPresent = NULL;
tReset oReset = NULL;
tEndScene oEndScene = NULL;

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDev, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {

	return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDev, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	//on lost - create and invalidate
	auto result = oReset(pDev, pPresentationParameters);
	//on reset - create and invalidate
	return result;
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev) {

	return oEndScene(pDev);
}

void InitDXHooks() {
	auto d3dVtable = GetD3DVtable(fwa(NULL, xc("CROSSFIRE")));
	if (d3dVtable) {
		oPresent = (tPresent)d3dVtable[_present];
		oReset = (tReset)d3dVtable[_reset];
		oEndScene = (tEndScene)d3dVtable[_endScene];
		if (MH_CreateHook((DWORD_PTR*)d3dVtable[_present], &hkPresent, reinterpret_cast<void**>(&oPresent)) != MH_OK) {
			//std::cout << "failed to create present hook" << std::endl;
		}
		if (MH_EnableHook((DWORD_PTR*)d3dVtable[_present]) != MH_OK) {
			//std::cout << "failed to enable present hook" << std::endl;
		}
		if (MH_CreateHook((DWORD_PTR*)d3dVtable[_reset], &hkReset, reinterpret_cast<void**>(&oReset)) != MH_OK) {
			//std::cout << "failed to create reset hook" << std::endl;
		}
		if (MH_EnableHook((DWORD_PTR*)d3dVtable[_reset]) != MH_OK) {
			//std::cout << "failed to enable reset hook" << std::endl;
		}
		if (MH_CreateHook((DWORD_PTR*)d3dVtable[_endScene], &hkEndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) {
			//std::cout << "failed to create end scene hook" << std::endl;
		}
		if (MH_EnableHook((DWORD_PTR*)d3dVtable[_endScene]) != MH_OK) {
			//std::cout << "failed to enable end scene hook" << std::endl;
		}
	}
}