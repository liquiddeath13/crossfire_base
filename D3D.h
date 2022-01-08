#pragma once

tPresent oPresent = NULL;
tReset oReset = NULL;
tEndScene oEndScene = NULL;

DWORD64 presentHookCalledTimes = 0;
DWORD64 endsceneHookCalledTimes = 0;

struct D3DEngine {
private:
	struct D3DTLVERTEX
	{
		float x, y, z, rhw;
		DWORD color;
	};
	DWORD64* d3dVtable = NULL;
	LPDIRECT3DDEVICE9 pDev = NULL;
	ID3DXFont* dx_Font = NULL;
	ID3DXFont* dx_FontStr = NULL;
	bool hideDrawings;
	DWORD64* GetD3DVtable(HWND hWnd)
	{
		if (d3dVtable) {
			return d3dVtable;
		}
		typedef LPDIRECT3D9(__stdcall* D3DCreate)(UINT);
		HMODULE hDLL = GetModuleBaseAddress(xw(L"d3d9.dll"));
		if (hDLL == nullptr) {
			if (DebugConsole->IsAttached()) {
				DebugConsole->PrintMsg(xc("!hD3D9dll"));
			}
			return nullptr;
		}
		D3DCreate pDirect3DCreate9 = (D3DCreate)GetExportAddress(hDLL, xc("Direct3DCreate9"));

		LPDIRECT3D9 pD3D = pDirect3DCreate9(D3D_SDK_VERSION);
		D3DDISPLAYMODE d3ddm;
		HRESULT hRes = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = true;
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		IDirect3DDevice9* ppReturnedDeviceInterface = nullptr;

		hRes = pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3dpp.hDeviceWindow,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &ppReturnedDeviceInterface
		);

		DWORD64* d3dVtable = NULL;

		if (ppReturnedDeviceInterface != nullptr) {
			d3dVtable = (DWORD64*)(*((DWORD64*)ppReturnedDeviceInterface));
		}

		pD3D->Release();
		return d3dVtable;
	}
	D3DTLVERTEX CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V) {
		return { X, Y, Z, RHW, color };
	}
public:
	void SetHideDrawings(bool state) {
		hideDrawings = state;
	}
	bool ShouldHide() {
		return hideDrawings;
	}
	bool HookPresent(LPVOID hkp, bool install = true) {
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("HookPresent started"));
		}
		auto d3dVtable = GetD3DVtable(fwa(NULL, xc("CROSSFIRE")));
		if (d3dVtable) {
			oPresent = (tPresent)d3dVtable[_present];
			if (install) {
				if (MH_CreateHook((DWORD_PTR*)d3dVtable[_present], hkp, reinterpret_cast<void**>(&oPresent)) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to create present hook"));
					}
					return false;
				}
				if (MH_EnableHook((DWORD_PTR*)d3dVtable[_present]) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to enable present hook"));
					}
					return false;
				}
			}
			else {
				if (MH_DisableHook((DWORD_PTR*)d3dVtable[_present]) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to disable present hook"));
					}
					return false;
				}
			}
			return true;
		}
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("failed to get d3dvt"));
		}
		return false;
	}
	bool HookEndScene(LPVOID hkes, bool install = true) {
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("HookEndScene started"));
		}
		auto d3dVtable = GetD3DVtable(fwa(NULL, xc("CROSSFIRE")));
		if (d3dVtable) {
			oEndScene = (tEndScene)d3dVtable[_endScene];
			if (install) {
				if (MH_CreateHook((DWORD_PTR*)d3dVtable[_endScene], hkes, reinterpret_cast<void**>(&oEndScene)) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to create end scene hook"));
					}
					return false;
				}
				if (MH_EnableHook((DWORD_PTR*)d3dVtable[_endScene]) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to enable end scene hook"));
					}
					return false;
				}
			}
			else {
				if (MH_DisableHook((DWORD_PTR*)d3dVtable[_endScene]) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to disable reset hook"));
					}
					return false;
				}
			}
			return true;
		}
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("failed to get d3dvt"));
		}
		return false;
	}
	bool HookReset(LPVOID hkr, bool install = true) {
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("HookReset started"));
		}
		auto d3dVtable = GetD3DVtable(fwa(NULL, xc("CROSSFIRE")));
		if (d3dVtable) {
			oReset = (tReset)d3dVtable[_reset];
			if (install) {
				if (MH_CreateHook((DWORD_PTR*)d3dVtable[_reset], hkr, reinterpret_cast<void**>(&oReset)) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to create reset hook"));
					}
					return false;
				}
				if (MH_EnableHook((DWORD_PTR*)d3dVtable[_reset]) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to enable reset hook"));
					}
					return false;
				}
			}
			else {
				if (MH_DisableHook((DWORD_PTR*)d3dVtable[_reset]) != MH_OK) {
					if (DebugConsole->IsAttached()) {
						DebugConsole->PrintMsg(xc("failed to disable reset hook"));
					}
					return false;
				}
			}
			return true;
		}
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("failed to get d3dvt"));
		}
		return false;
	}
	void SetDevice(LPDIRECT3DDEVICE9 dev) {
		pDev = dev;
	}
	void HandleOnLost() {
		if (dx_Font) {
			dx_Font->OnLostDevice();
		}
		if (dx_FontStr) {
			dx_FontStr->OnLostDevice();
		}
	}
	void HandleOnReset() {
		if (dx_Font) {
			dx_Font->OnResetDevice();
		}
		if (dx_FontStr) {
			dx_FontStr->OnResetDevice();
		}
	}
	void DrawTxt(int x, int y, LPCSTR TextToDraw, D3DCOLOR color)
	{
		if (hideDrawings) return;
		if (!dx_Font) {
			D3DXCreateFontA(pDev, 16, 6, FW_DONTCARE, 0, 0, RUSSIAN_CHARSET, OUT_TT_ONLY_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, xc("Tahoma"), &dx_Font);
		}
		tagRECT pos{ x, y, 100, 100 };
		dx_Font->DrawTextA(NULL, TextToDraw, strlen(TextToDraw), &pos, DT_NOCLIP, color);
	}
	void FillRGB(INT x, INT y, INT w, INT h, D3DCOLOR color)
	{
		if (hideDrawings) return;
		D3DRECT rec = { x, y, x + w, y + h };
		pDev->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
	}
	void DrawBorder(INT x, INT y, INT w, INT h, INT px, D3DCOLOR BorderColor)
	{
		if (hideDrawings) return;
		FillRGB(x, (y + h - px), w, px, BorderColor);
		FillRGB(x, y, px, h, BorderColor);
		FillRGB(x, y, w, px, BorderColor);
		FillRGB((x + w - px), y, px, h, BorderColor);
	}
	void DrawCircle(int x, int y, float radius, D3DCOLOR color)
	{
		if (hideDrawings) return;
		const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
		const int NUMPOINTS = 30;
		const float PI = (float)3.141592654f;

		D3DTLVERTEX Circle[NUMPOINTS + 1];
		float X, Y, Theta, WedgeAngle;
		WedgeAngle = (float)((2 * PI) / NUMPOINTS);

		for (int i = 0; i <= NUMPOINTS; i++)
		{
			Theta = i * WedgeAngle;
			X = (float)(x + radius * cos(Theta));
			Y = (float)(y - radius * sin(Theta));
			Circle[i] = CreateD3DTLVERTEX(X, Y, 0.0f, 1.0f, color, 0.0f, 0.0f);
		}

		pDev->SetRenderState(D3DRS_ZENABLE, 0);
		pDev->SetRenderState(D3DRS_FOGENABLE, false);
		pDev->SetRenderState(D3DRS_LIGHTING, false);
		pDev->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		pDev->SetFVF(D3D_FVF);
		pDev->SetTexture(0, NULL);
		pDev->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &Circle[0], sizeof(Circle[0]));
		pDev->SetTexture(0, NULL);
	}
	void DrawLine(float X, float Y, float X2, float Y2, DWORD dwColor)
	{
		if (hideDrawings) return;
		if (!pDev) return;
		D3DTLVERTEX qV[2] =
		{
			{ (float)X, (float)Y, 0.0f, 1.0f, dwColor },
			{ (float)X2, (float)Y2, 0.0f, 1.0f, dwColor },
		};

		pDev->SetTexture(0, NULL);
		pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, 1);
		pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pDev->SetRenderState(D3DRS_FOGENABLE, false);
		pDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		pDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, qV, sizeof(D3DTLVERTEX));
	}
	void Gradient(float x, float y, float w, float h, DWORD ColorStart, DWORD ColorEnd, GradientOrientation Orientation)
	{
		if (hideDrawings) return;
		const DWORD D3DGRADE_ALLCOMPUTER = 0x004 | 0x040;
		struct D3DVERTEX { float x, y, z, rhw; DWORD COR; }

		vertices[4] = {
			{ 0, 0, 0, 1.0f, 0 },
			{ 0, 0, 0, 1.0f, 0 },
			{ 0, 0, 0, 1.0f, 0 },
			{ 0, 0, 0, 1.0f, 0 }
		};

		vertices[0].x = x;
		vertices[0].y = y;
		vertices[0].COR = ColorStart;

		vertices[1].x = x + w;
		vertices[1].y = y;
		vertices[1].COR = Orientation == GradientOrientation::Vertical ? ColorEnd : ColorStart;

		vertices[2].x = x;
		vertices[2].y = y + h;
		vertices[2].COR = Orientation == GradientOrientation::Vertical ? ColorStart : ColorEnd;

		vertices[3].x = x + w;
		vertices[3].y = y + h;
		vertices[3].COR = ColorEnd;

		pDev->SetTexture(0, 0);
		pDev->SetRenderState(D3DRS_ALPHATESTENABLE, D3DZB_TRUE);
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_TRUE);
		pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL); //D3DCMP_GREATEREQUAL
		pDev->SetRenderState(D3DRS_ALPHAREF, (DWORD)8); // (DWORD)8
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pDev->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);
		pDev->SetFVF(0x004 | 0x040 | 0X100);
		pDev->SetFVF(D3DGRADE_ALLCOMPUTER);
		pDev->SetTexture(0, 0);
		pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVERTEX));
	}
	void DrawBox(INT X, INT Y, INT W, INT H, D3DCOLOR Color, D3DCOLOR BorderColor)
	{
		if (hideDrawings) return;
		DrawBorder(X, Y, W, H, 1, BorderColor);
		CONST DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
		struct Vertex { FLOAT X, Y, W, H;	D3DCOLOR Color; }

		V[4] = {
			{ (FLOAT)X, (FLOAT)(Y + H), 0.0F, 0.0F, Color },
			{ (FLOAT)X, (FLOAT)Y, 0.0F, 0.0F, Color },
			{ (FLOAT)(X + W), (FLOAT)(Y + H), 0.0F, 0.0F, Color },
			{ (FLOAT)(X + W), (FLOAT)Y, 0.0F, 0.0F, Color }
		};

		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_TRUE);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pDev->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);

		pDev->SetFVF(D3D_FVF);
		pDev->SetTexture(0, NULL);
		pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
	}
	void DrawTriangle(INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, DWORD Color1, DWORD Color2, DWORD Color3)
	{
		if (hideDrawings) return;
		CONST DWORD dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
		D3DTLVERTEX V[3] =
		{
			{ (FLOAT)x1, (FLOAT)y1, 0.5f, 1.0f, Color1 },
			{ (FLOAT)x3, (FLOAT)y3, 0.5f, 1.0f, Color3 },
			{ (FLOAT)x2, (FLOAT)y2, 0.5f, 1.0f, Color2 },
		};

		VOID* pVertexBuffer = NULL;
		LPDIRECT3DVERTEXBUFFER9 pBuffer = NULL;

		if (FAILED(pDev->CreateVertexBuffer(3 * sizeof(D3DTLVERTEX), 0, dwFVF, D3DPOOL_DEFAULT, &pBuffer, NULL)))
			return;

		if (FAILED(pBuffer->Lock(0, sizeof(V), &pVertexBuffer, 0)))
			return;

		memcpy(pVertexBuffer, V, sizeof(V));
		pBuffer->Unlock();

		pDev->SetFVF(dwFVF);
		pDev->SetStreamSource(0, pBuffer, 0, sizeof(D3DTLVERTEX));
		pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
	}

	void DrawBone(Player* player, UINT iStart, UINT iEnd, D3DCOLOR Color)
	{
		if (hideDrawings) return;
		auto StartPos = player->GetNodeTransform(iStart).m_Pos.ToDX();
		auto EndPos = player->GetNodeTransform(iEnd).m_Pos.ToDX();

		if (WorldToScreen(pDev, &StartPos) && WorldToScreen(pDev, &EndPos)) DrawLine(StartPos.x, StartPos.y, EndPos.x, EndPos.y, Color);
	}
	void DrawCrosshair(D3DCOLOR Color, bool cross, bool dot, bool circle)
	{
		if (hideDrawings) return;
		/* Get center of screen */
		D3DVIEWPORT9 pViewport;
		pDev->GetViewport(&pViewport);

		DWORD cx = pViewport.Width / 2;
		DWORD cy = pViewport.Height / 2;

		if (circle) DrawCircle(cx, cy, 8, Color); //Circle

		if (cross) {
			FillRGB(cx - 13, cy, 10, 1, Color); //Left line
			FillRGB(cx + 4, cy, 10, 1, Color); //Right line
			FillRGB(cx, cy - 13, 1, 10, Color); //Top line
			FillRGB(cx, cy + 4, 1, 10, Color); //Bottom line
		}

		if (dot) FillRGB(cx, cy, 1, 1, Color); //Dot point
	}
	void DrawSkeleton(Player* player, bool withNickname, D3DCOLOR color) {
		if (hideDrawings) return;
		auto tHead = player->GetNodeTransform(6).m_Pos.ToDX();
		auto tNeck = player->GetNodeTransform(5).m_Pos.ToDX();
		if (!WorldToScreen(pDev, &tHead) || !WorldToScreen(pDev, &tNeck)) return;
		int HeadRadius = GetBoneRadius(tNeck, tHead);
		int nameX = (int)tHead.x - strlen(player->Nickname) * 6;
		int nameY = (int)tHead.y - 13;

		if (withNickname) DrawTxt(nameX, nameY, player->Nickname, color);

		DrawCircle(tHead.x, tHead.y - HeadRadius, HeadRadius, color);
		//auto me = PlayerList::Me();
		//if (me.IsOpponent(player->TeamIndex) && PlayerList::IsVisible(me, *player) != -1) {
		//	color = D3DCOLOR_COLORVALUE(0, 1.f, 0.f, 1.f);
		//}
		DrawBone(player, 6, 0, color);
		DrawBone(player, 5, 6, color);
		DrawBone(player, 4, 5, color);
		DrawBone(player, 3, 4, color);
		DrawBone(player, 2, 3, color);
		DrawBone(player, 1, 2, color);

		DrawBone(player, 21, 1, color);
		DrawBone(player, 22, 21, color);
		DrawBone(player, 23, 22, color);
		DrawBone(player, 24, 23, color);

		DrawBone(player, 25, 1, color);
		DrawBone(player, 26, 25, color);
		DrawBone(player, 27, 26, color);
		DrawBone(player, 28, 27, color);

		DrawBone(player, 14, 5, color);
		DrawBone(player, 15, 14, color);
		DrawBone(player, 16, 15, color);
		DrawBone(player, 17, 16, color);
		DrawBone(player, 18, 17, color);
		DrawBone(player, 19, 17, color);
		DrawBone(player, 20, 17, color);

		DrawBone(player, 7, 5, color);
		DrawBone(player, 8, 7, color);
		DrawBone(player, 9, 8, color);
		DrawBone(player, 10, 9, color);
		DrawBone(player, 11, 10, color);
		DrawBone(player, 12, 10, color);
		DrawBone(player, 13, 10, color);
	}
	void DrawHealthBar(Player* Player, int x, int y)
	{
		if (hideDrawings) return;
		DWORD healthColor = 0xFF00FF00;
		int health = (Player->Health > 0) ? Player->Health : 100;

		// Get Health color
		if (health > 80) healthColor = 0xFF00FF00;
		else if (health > 60) healthColor = 0xFFFFFF00;
		else if (health > 40) healthColor = 0xFFFF4500;
		else healthColor = 0xFFFF0000;

		int BoxWidth = 30;

		// Bar width / position fix
		int BarWidth = (int)((health * BoxWidth) / 100);
		int barDiff = (BoxWidth - BarWidth);

		// Draw health background
		FillRGB(x + 5, y - 2, BoxWidth + 1, 5, 0xFF000000);

		// Draw health fill
		FillRGB(x + 4, y - 1, BarWidth, 3, healthColor);
	}
	void DrawBox2D(DWORD ESP_Color, Player* Player)
	{
		if (hideDrawings) return;
		D3DXVECTOR3 MidPosition, pLeftTop, pLeftBottom, pRightBottom, pRightTop;

		MidPosition = GetMidPoint(Player->GetBoneTransform(6).m_Pos.ToDX(), Player->GetBoneTransform(3).m_Pos.ToDX());
		pLeftTop = MidPosition + D3DXVECTOR3(-50, 160, 0);  // <- Left Top
		pLeftBottom = MidPosition + D3DXVECTOR3(-50, -160, 0); // <- Left Bottom
		pRightBottom = MidPosition + D3DXVECTOR3(50, -160, 0);  // <- Right Bottom
		pRightTop = MidPosition + D3DXVECTOR3(50, 160, 0);   // <- Right Top

		if (WorldToScreen(pDev, &pLeftTop) && WorldToScreen(pDev, &pLeftBottom) && WorldToScreen(pDev, &pRightBottom) && WorldToScreen(pDev, &pRightTop))
		{
			DrawLine(pLeftBottom.x, pLeftBottom.y, pRightBottom.x, pRightBottom.y, ESP_Color); // ___ 
			DrawLine(pLeftTop.x, pLeftTop.y, pLeftBottom.x, pLeftBottom.y, ESP_Color);         // |  
			DrawLine(pRightBottom.x, pRightBottom.y, pRightTop.x, pRightTop.y, ESP_Color);     //   | 
			DrawLine(pRightTop.x, pRightTop.y, pLeftTop.x, pLeftTop.y, ESP_Color);             // --- 
		}
	}
	void DrawHP(Player* Player) {
		if (hideDrawings) return;
		auto head = Player->GetBoneTransform(6).m_Pos.ToDX();
		if (WorldToScreen(pDev, &head)) {
			DrawHealthBar(Player, head.x - 15, head.y - 10);
		}
	}
};

D3DEngine* D3D = new D3DEngine();

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDev, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
	if (D3D->ShouldHide()) {
		return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	D3D->SetDevice(pDev);
	D3D->DrawTxt(20, 20, std::string(xc("present hook called ") + std::to_string(++presentHookCalledTimes) + xc(" times")).c_str(), Gold);
	//PLACE AIMBOT, ESP, MENU HERE, BECAUSE PRESENT CALLED A LITTLE MORE THAN ENDSCENE AND OBS CAN'T CAPTURE THAT IF CHECKED "DON'T CAPTURE EXTERNAL OVERLAYS"
	auto me = PlayersMgr->Me();
	if (me != nullptr && me->IsValid()) {
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(xc("we are in fight, looping through players list..."));
		}
		auto playersListInfo = PlayersMgr->RetrieveAll();
		switch (me->IsVisibleByOpponents(playersListInfo.first, me, playersListInfo.second)) {
		case VISIBLE_BY::Opponent:
			D3D->DrawTxt(50, 50, xc("YOU'RE VISIBLE BY OPPONENT"), Red);
			break;
		}
		for (size_t i = 0; i < playersListInfo.second; i++)
		{
			auto player = playersListInfo.first[i];
			if (player != nullptr && player->IsValid() && player->ClientID != me->ClientID) {
				D3DCOLOR color;
				if (player->TeamIndex == me->TeamIndex) {
					//teammate

				}
				else {
					//opponent
					switch (player->IsVisibleByOpponents(playersListInfo.first, me, playersListInfo.second)) {
					case VISIBLE_BY::Me:
						color = D3DCOLOR_COLORVALUE(0.f, 1.f, 0.f, 1.f);
						break;
					case VISIBLE_BY::Teammate:
						color = D3DCOLOR_COLORVALUE(1.f, 1.f, 1.f, 1.f);
						break;
					default:
						color = D3DCOLOR_COLORVALUE(1.f, 0.f, 0.f, 1.f);
						break;
					}
					D3D->DrawSkeleton(player, true, color);
				}
			}
		}
	}
	return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDev, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	D3D->SetDevice(pDev);
	D3D->HandleOnLost();
	auto result = oReset(pDev, pPresentationParameters);
	D3D->HandleOnReset();
	return result;
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev) {
	if (D3D->ShouldHide()) {
		return oEndScene(pDev);
	}
	D3D->SetDevice(pDev);
	D3D->DrawTxt(20, 25, std::string(xc("endscene hook called ") + std::to_string(++endsceneHookCalledTimes) + xc(" times")).c_str(), White);
	return oEndScene(pDev);
}

bool InitDXHooks() {
	return D3D->HookPresent(&hkPresent) && D3D->HookReset(&hkReset) && D3D->HookEndScene(&hkEndScene);
}

bool DeinitDXHooks() {
	return D3D->HookPresent(NULL, false) && D3D->HookReset(NULL, false) && D3D->HookEndScene(NULL, false);
}