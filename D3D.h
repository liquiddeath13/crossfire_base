#pragma once

tPresent oPresent = NULL;
tReset oReset = NULL;
tEndScene oEndScene = NULL;

#pragma region drawings
enum GradientOrientation
{
	Horizontal,
	Vertical
};

ID3DXFont* dx_Font;
void dxDrawText(IDirect3DDevice9* pDev, int x, int y, LPCSTR TextToDraw, D3DCOLOR color)
{
	if (!dx_Font) {
		D3DXCreateFontA(pDev, 16, 6, FW_DONTCARE, 0, 0, RUSSIAN_CHARSET, OUT_TT_ONLY_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, xc("Tahoma"), &dx_Font);
	}
	tagRECT pos{ x, y, 100, 100 };
	dx_Font->DrawTextA(NULL, TextToDraw, strlen(TextToDraw), &pos, DT_NOCLIP, color);
}

ID3DXFont* dx_FontStr;
void dxDrawStr(IDirect3DDevice9* pDev, int x, int y, LPCSTR TextToDraw, D3DCOLOR color) {
	if (!dx_FontStr) {
		D3DXCreateFontA(pDev, 18, 10, FW_DONTCARE, 0, 0, RUSSIAN_CHARSET, OUT_TT_ONLY_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, xc("Tahoma"), &dx_FontStr);
	}
	tagRECT pos{ x, y, 100, 100 };
	dx_FontStr->DrawTextA(NULL, TextToDraw, strlen(TextToDraw), &pos, DT_NOCLIP, color);
}

struct D3DTLVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

D3DTLVERTEX CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V) { D3DTLVERTEX v = { X, Y, Z, RHW, color }; return v; }

void DrawCircle(int x, int y, float radius, D3DCOLOR color, IDirect3DDevice9* pDevice)
{
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

	pDevice->SetRenderState(D3DRS_ZENABLE, 0);
	pDevice->SetRenderState(D3DRS_FOGENABLE, false);
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetFVF(D3D_FVF);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &Circle[0], sizeof(Circle[0]));
	pDevice->SetTexture(0, NULL);
}

void DrawLine(LPDIRECT3DDEVICE9 pDevice, float X, float Y, float X2, float Y2, DWORD dwColor)
{
	if (!pDevice) return;
	D3DTLVERTEX qV[2] =
	{
		{ (float)X, (float)Y, 0.0f, 1.0f, dwColor },
		{ (float)X2, (float)Y2, 0.0f, 1.0f, dwColor },
	};

	pDevice->SetTexture(0, NULL);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, 1);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE, false);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, qV, sizeof(D3DTLVERTEX));
}

void DrawBone(LPDIRECT3DDEVICE9 pDevice, Player* player, UINT iStart, UINT iEnd, D3DCOLOR Color)
{
	auto StartPos = player->GetNodeTransform(iStart).m_Pos.ToDX();
	auto EndPos = player->GetNodeTransform(iEnd).m_Pos.ToDX();

	if (WorldToScreen(pDevice, &StartPos) && WorldToScreen(pDevice, &EndPos)) DrawLine(pDevice, StartPos.x, StartPos.y, EndPos.x, EndPos.y, Color);
}

size_t GetBoneRadius(LPDIRECT3DDEVICE9 pDev, Player* player, LTVector<float> start, LTVector<float> end) {
	D3DXVECTOR3 Box = D3DXVECTOR3(end.x, end.y, end.z) - D3DXVECTOR3(start.x, start.y, start.z);
	if (Box.y < 0) Box.y *= -1;
	return (int)Box.y * 1.5;
}

void FillRGB(INT x, INT y, INT w, INT h, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorder(INT x, INT y, INT w, INT h, INT px, D3DCOLOR BorderColor, LPDIRECT3DDEVICE9 pDevice)
{
	FillRGB(x, (y + h - px), w, px, BorderColor, pDevice);
	FillRGB(x, y, px, h, BorderColor, pDevice);
	FillRGB(x, y, w, px, BorderColor, pDevice);
	FillRGB((x + w - px), y, px, h, BorderColor, pDevice);
}

void Gradient(float x, float y, float w, float h, DWORD ColorStart, DWORD ColorEnd, GradientOrientation Orientation, LPDIRECT3DDEVICE9 pDevice)
{
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
	vertices[1].COR = Orientation == Vertical ? ColorEnd : ColorStart;

	vertices[2].x = x;
	vertices[2].y = y + h;
	vertices[2].COR = Orientation == Vertical ? ColorStart : ColorEnd;

	vertices[3].x = x + w;
	vertices[3].y = y + h;
	vertices[3].COR = ColorEnd;

	pDevice->SetTexture(false, 0);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, D3DZB_TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL); //D3DCMP_GREATEREQUAL
	pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)8); // (DWORD)8
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);
	pDevice->SetFVF(0x004 | 0x040 | 0X100);
	pDevice->SetFVF(D3DGRADE_ALLCOMPUTER);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVERTEX));
}

void DrawBox(INT X, INT Y, INT W, INT H, D3DCOLOR Color, D3DCOLOR BorderColor, LPDIRECT3DDEVICE9 pDevice)
{

	DrawBorder(X, Y, W, H, 1, BorderColor, pDevice);
	CONST DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	struct Vertex { FLOAT X, Y, W, H;	D3DCOLOR Color; }

	V[4] = {
		{ (FLOAT)X, (FLOAT)(Y + H), 0.0F, 0.0F, Color },
		{ (FLOAT)X, (FLOAT)Y, 0.0F, 0.0F, Color },
		{ (FLOAT)(X + W), (FLOAT)(Y + H), 0.0F, 0.0F, Color },
		{ (FLOAT)(X + W), (FLOAT)Y, 0.0F, 0.0F, Color }
	};

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_TRUE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);

	pDevice->SetFVF(D3D_FVF);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
}

void DrawTriangle(INT x1, INT y1, INT x2, INT y2, INT x3, INT y3, DWORD Color1, DWORD Color2, DWORD Color3, LPDIRECT3DDEVICE9 pDevice)
{
	CONST DWORD dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	D3DTLVERTEX V[3] =
	{
		{ (FLOAT)x1, (FLOAT)y1, 0.5f, 1.0f, Color1 },
		{ (FLOAT)x3, (FLOAT)y3, 0.5f, 1.0f, Color3 },
		{ (FLOAT)x2, (FLOAT)y2, 0.5f, 1.0f, Color2 },
	};

	VOID* pVertexBuffer = NULL;
	LPDIRECT3DVERTEXBUFFER9 pBuffer = NULL;

	if (FAILED(pDevice->CreateVertexBuffer(3 * sizeof(D3DTLVERTEX), 0, dwFVF, D3DPOOL_DEFAULT, &pBuffer, NULL)))
		return;

	if (FAILED(pBuffer->Lock(0, sizeof(V), &pVertexBuffer, 0)))
		return;

	memcpy(pVertexBuffer, V, sizeof(V));
	pBuffer->Unlock();

	pDevice->SetFVF(dwFVF);
	pDevice->SetStreamSource(0, pBuffer, 0, sizeof(D3DTLVERTEX));
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

void DrawCrosshair(LPDIRECT3DDEVICE9 pDevice, D3DCOLOR Color, bool cross, bool dot, bool circle)
{
	/* Get center of screen */
	D3DVIEWPORT9 pViewport;
	pDevice->GetViewport(&pViewport);

	DWORD cx = pViewport.Width / 2;
	DWORD cy = pViewport.Height / 2;

	if (circle) DrawCircle(cx, cy, 8, Color, pDevice); //Circle

	if (cross) {
		FillRGB(cx - 13, cy, 10, 1, Color, pDevice); //Left line
		FillRGB(cx + 4, cy, 10, 1, Color, pDevice); //Right line
		FillRGB(cx, cy - 13, 1, 10, Color, pDevice); //Top line
		FillRGB(cx, cy + 4, 1, 10, Color, pDevice); //Bottom line
	}

	if (dot) FillRGB(cx, cy, 1, 1, Color, pDevice); //Dot point
}

void DrawHealthBar(LPDIRECT3DDEVICE9 pDevice, Player* Player, int x, int y)
{
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
	FillRGB(x + 5, y - 2, BoxWidth + 1, 5, 0xFF000000, pDevice);

	// Draw health fill
	FillRGB(x + 4, y - 1, BarWidth, 3, healthColor, pDevice);
}

LTObject* ObjToLTObj(void* obj) {
	return (LTObject*)obj;
}

void DrawSkeleton(LPDIRECT3DDEVICE9 pDev, Player* player, bool withNickname, bool withKills, bool withDeaths, bool withHeadshots, bool withPing) {
	D3DCOLOR color = D3DCOLOR_COLORVALUE(0.f, 0.f, 1.f, 1.f);
	auto obj = ObjToLTObj(player->Object);
	if (player->TeamIndex == 0) {
		color = D3DCOLOR_COLORVALUE(1.f, 0.f, 0.f, 1.f);
	}
	auto tHead = player->GetNodeTransform(6).m_Pos.ToDX();
	auto tNeck = player->GetNodeTransform(5).m_Pos.ToDX();
	if (!WorldToScreen(pDev, &tHead) || !WorldToScreen(pDev, &tNeck)) return;
	D3DXVECTOR3 Box = D3DXVECTOR3(tNeck.x, tNeck.y, tNeck.z) - D3DXVECTOR3(tHead.x, tHead.y, tHead.z);
	if (Box.y < 0) Box.y *= -1;
	int HeadRadius = (int)Box.y * 1.5;
	int nameX = (int)tHead.x - strlen(player->Nickname) * 6;
	int nameY = (int)tHead.y - 13;

	if (withNickname) dxDrawText(pDev, nameX, nameY, player->Nickname, color);

	DrawCircle(tHead.x, tHead.y - HeadRadius, HeadRadius, color, pDev);
	//auto me = PlayerList::Me();
	//if (me.IsOpponent(player->TeamIndex) && PlayerList::IsVisible(me, *player) != -1) {
	//	color = D3DCOLOR_COLORVALUE(0, 1.f, 0.f, 1.f);
	//}
	DrawBone(pDev, player, 6, 0, color);
	DrawBone(pDev, player, 5, 6, color);
	DrawBone(pDev, player, 4, 5, color);
	DrawBone(pDev, player, 3, 4, color);
	DrawBone(pDev, player, 2, 3, color);
	DrawBone(pDev, player, 1, 2, color);

	DrawBone(pDev, player, 21, 1, color);
	DrawBone(pDev, player, 22, 21, color);
	DrawBone(pDev, player, 23, 22, color);
	DrawBone(pDev, player, 24, 23, color);

	DrawBone(pDev, player, 25, 1, color);
	DrawBone(pDev, player, 26, 25, color);
	DrawBone(pDev, player, 27, 26, color);
	DrawBone(pDev, player, 28, 27, color);

	DrawBone(pDev, player, 14, 5, color);
	DrawBone(pDev, player, 15, 14, color);
	DrawBone(pDev, player, 16, 15, color);
	DrawBone(pDev, player, 17, 16, color);
	DrawBone(pDev, player, 18, 17, color);
	DrawBone(pDev, player, 19, 17, color);
	DrawBone(pDev, player, 20, 17, color);

	DrawBone(pDev, player, 7, 5, color);
	DrawBone(pDev, player, 8, 7, color);
	DrawBone(pDev, player, 9, 8, color);
	DrawBone(pDev, player, 10, 9, color);
	DrawBone(pDev, player, 11, 10, color);
	DrawBone(pDev, player, 12, 10, color);
	DrawBone(pDev, player, 13, 10, color);
}

void DrawBox2D(LPDIRECT3DDEVICE9 pDevice, DWORD ESP_Color, Player* Player)
{
	D3DXVECTOR3 MidPosition, pLeftTop, pLeftBottom, pRightBottom, pRightTop;

	MidPosition = GetMidPoint(Player->GetBoneTransform(6).m_Pos.ToDX(), Player->GetBoneTransform(3).m_Pos.ToDX());
	pLeftTop = MidPosition + D3DXVECTOR3(-50, 160, 0);  // <- Left Top
	pLeftBottom = MidPosition + D3DXVECTOR3(-50, -160, 0); // <- Left Bottom
	pRightBottom = MidPosition + D3DXVECTOR3(50, -160, 0);  // <- Right Bottom
	pRightTop = MidPosition + D3DXVECTOR3(50, 160, 0);   // <- Right Top

	if (WorldToScreen(pDevice, &pLeftTop) && WorldToScreen(pDevice, &pLeftBottom) && WorldToScreen(pDevice, &pRightBottom) && WorldToScreen(pDevice, &pRightTop))
	{
		DrawLine(pDevice, pLeftBottom.x, pLeftBottom.y, pRightBottom.x, pRightBottom.y, ESP_Color); // ___ 
		DrawLine(pDevice, pLeftTop.x, pLeftTop.y, pLeftBottom.x, pLeftBottom.y, ESP_Color);         // |  
		DrawLine(pDevice, pRightBottom.x, pRightBottom.y, pRightTop.x, pRightTop.y, ESP_Color);     //   | 
		DrawLine(pDevice, pRightTop.x, pRightTop.y, pLeftTop.x, pLeftTop.y, ESP_Color);             // --- 
	}
}

void DrawHP(IDirect3DDevice9* pDev, Player* Player) {
	auto head = Player->GetBoneTransform(6).m_Pos.ToDX();
	if (WorldToScreen(pDev, &head)) {
		DrawHealthBar(pDev, Player, head.x - 15, head.y - 10);
	}
}

#pragma endregion

DWORD64 presentHookCalledTimes = 0;
DWORD64 endsceneHookCalledTimes = 0;

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDev, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
	dxDrawStr(pDev, 20, 20, std::string(xc("present hook called ") + std::to_string(++presentHookCalledTimes) + xc(" times")).c_str(), Gold);
	//PLACE AIMBOT, ESP, MENU HERE, BECAUSE PRESENT CALLED A LITTLE MORE THAN ENDSCENE AND OBS CAN'T CAPTURE THAT IF CHECKED "DON'T CAPTURE EXTERNAL OVERLAYS"
	return oPresent(pDev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDev, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	if (dx_Font) dx_Font->OnLostDevice();
	if (dx_FontStr) dx_FontStr->OnLostDevice();
	auto result = oReset(pDev, pPresentationParameters);
	if (dx_Font) dx_Font->OnResetDevice();
	if (dx_FontStr) dx_FontStr->OnResetDevice();
	return result;
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev) {
	dxDrawStr(pDev, 20, 25, std::string(xc("endscene hook called ") + std::to_string(++endsceneHookCalledTimes) + xc(" times")).c_str(), White);
	return oEndScene(pDev);
}

bool InitDXHooks() {
	auto d3dVtable = GetD3DVtable(fwa(NULL, xc("CROSSFIRE")));
	if (d3dVtable) {
		oPresent = (tPresent)d3dVtable[_present];
		oReset = (tReset)d3dVtable[_reset];
		oEndScene = (tEndScene)d3dVtable[_endScene];
		if (MH_CreateHook((DWORD_PTR*)d3dVtable[_present], &hkPresent, reinterpret_cast<void**>(&oPresent)) != MH_OK) {
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
		if (MH_CreateHook((DWORD_PTR*)d3dVtable[_reset], &hkReset, reinterpret_cast<void**>(&oReset)) != MH_OK) {
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
		if (MH_CreateHook((DWORD_PTR*)d3dVtable[_endScene], &hkEndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) {
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
		return true;
	}
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(xc("failed to get d3dvt"));
	}
	return false;
}

void DeinitDXHooks() {
	auto d3dVtable = GetD3DVtable(fwa(NULL, xc("CROSSFIRE")));
	if (d3dVtable) {
		if (MH_DisableHook((DWORD_PTR*)d3dVtable[_present]) != MH_OK) {
			//std::cout << "failed to disable present hook" << std::endl;
		}
		if (MH_DisableHook((DWORD_PTR*)d3dVtable[_reset]) != MH_OK) {
			//std::cout << "failed to disable reset hook" << std::endl;
		}
		if (MH_DisableHook((DWORD_PTR*)d3dVtable[_endScene]) != MH_OK) {
			//std::cout << "failed to disable reset hook" << std::endl;
		}
	}
}