#pragma once

struct GameInfo {
	POINT ScreenCenter{};
	std::unordered_map<std::string, std::string> DebugLines;
	std::vector<std::string> VisibleBy;
	FOV TriggerBotFOV;
	FOV AimbotFOV;
	FOV SilentAimFOV;
	D3DCOLOR DebugStrColor;
	D3DCOLOR VisibleByColor;
	POINT DebugDrawPos;
	GameInfo(): VisibleBy(16, xc("")) {
		DebugDrawPos = {10, 20};
		DebugStrColor = White;
	}
	void SetDebugLine(std::string k, std::string v) {
		DebugLines[k] = v;
	}
	void AppendVisibleByList(std::string nickname, size_t pos) {
		VisibleBy[pos] = nickname;
	}
	void ClearVisibleByList() {
		VisibleBy.clear();
	}
};

GameInfo* GI = new GameInfo();