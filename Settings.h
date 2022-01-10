#pragma once

struct SettingsEngine {
private:
	std::unordered_map<std::string, bool> b_vals;
	std::unordered_map<std::string, int> i_vals;
	std::unordered_map<std::string, float> f_vals;
public:
	void SetBool(std::string name, bool val) {
		b_vals[name] = val;
	}
	bool GetBool(std::string name) {
		return b_vals[name];
	}
	void SetInt(std::string name, int val) {
		i_vals[name] = val;
	}
	int GetInt(std::string name) {
		return i_vals[name];
	}
	void SetFloat(std::string name, float val) {
		f_vals[name] = val;
	}
	float GetFloat(std::string name) {
		return f_vals[name];
	}
};

SettingsEngine* Settings;

void InitSettings() {
	Settings = new SettingsEngine();
	Settings->SetBool(xc("PreventFpsDrop"), false);
	Settings->SetInt(xc("DisableGhostESPAtPlayersCount"), 12);
	Settings->SetBool(xc("UseDebugConsole"), true);
	Settings->SetBool(xc("Skeletons"), true);
	Settings->SetBool(xc("2DBoxes"), false);
	Settings->SetBool(xc("LinesFromCenterToHead"), true);
	Settings->SetBool(xc("SkeletonsWithNicknames"), true);
	Settings->SetBool(xc("SkeletonsWithBoneLines"), true);
	Settings->SetBool(xc("SkeletonsWithDistance"), false);
	Settings->SetBool(xc("SkeletonsWithKills"), false);
	Settings->SetBool(xc("SkeletonsWithHealth"), false);
	Settings->SetBool(xc("SkeletonsWithBoneNumbers"), false);
	Settings->SetBool(xc("SkeletonsWithRadiusCircles"), false);
	Settings->SetBool(xc("GhostESP"), true);
	Settings->SetBool(xc("DrawRadius"), true);
	Settings->SetBool(xc("Aimbot"), true);
	Settings->SetBool(xc("AutoAim"), false);
	Settings->SetFloat(xc("AimSmooth"), 4.f);
	Settings->SetInt(xc("AimSearchType"), (int)AimSearchType::ClosestByDistance);
	Settings->SetInt(xc("AimKey"), VK_RBUTTON);
	Settings->SetBool(xc("TriggerBot"), true);
	Settings->SetBool(xc("AutoTrigger"), false);
	Settings->SetInt(xc("TriggerKey"), VK_LSHIFT);
	Settings->SetInt(xc("AimbotRadius"), 20);
	Settings->SetInt(xc("TriggerBotRadius"), 9);
	Settings->SetBool(xc("ESP"), true);
	Settings->SetBool(xc("YouAreVisibleAlarm"), true);
}