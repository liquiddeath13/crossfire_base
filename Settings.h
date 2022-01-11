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
	Settings->SetBool(xc("CountPerformance"), false);
	Settings->SetBool(xc("PreventFpsDrop"), true);
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
	Settings->SetBool(xc("DrawRadius"), true);
	Settings->SetBool(xc("Aimbot"), true);
	Settings->SetBool(xc("AutoAim"), true);
	Settings->SetFloat(xc("AimSmooth"), 1.35f);
	Settings->SetInt(xc("AimSearchType"), (int)AimSearchType::ClosestByDistance);
	Settings->SetBool(xc("TriggerBot"), true);
	Settings->SetBool(xc("AutoTrigger"), true);
	Settings->SetInt(xc("AimKey"), VK_LSHIFT);
	Settings->SetInt(xc("AimKeyONOFF"), VK_SCROLL);
	Settings->SetInt(xc("TriggerKey"), VK_RBUTTON);
	Settings->SetInt(xc("TriggerKeyONOFF"), VK_PAUSE);
	Settings->SetInt(xc("AimbotRadius"), 40);
	Settings->SetInt(xc("TriggerBotRadius"), 8);
	Settings->SetBool(xc("ESP"), true);
	/*Settings->SetFloat(xc("WorkMaxDistance"), 11000.f);
	Settings->SetFloat(xc("ESPMaxDistance"), 11000.f);
	Settings->SetFloat(xc("AimMaxDistance"), 11000.f);
	Settings->SetFloat(xc("TriggerBotMaxDistance"), 11000.f);*/
	Settings->SetBool(xc("YouAreVisibleAlarm"), true);
}