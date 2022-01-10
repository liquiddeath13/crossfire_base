#pragma once

struct GameInfo {
	FOV AimFOV{};
	FOV TriggerBotFOV{};
};

GameInfo* GI = new GameInfo();