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

	Settings->SetFloat(xc("AimSmooth"), 2.6f);

	/*Settings->SetFloat(xc("WorkMaxDistance"), 11000.f);
	Settings->SetFloat(xc("ESPMaxDistance"), 11000.f);
	Settings->SetFloat(xc("AimMaxDistance"), 11000.f);
	Settings->SetFloat(xc("TriggerBotMaxDistance"), 11000.f);*/
	
	Settings->SetBool(xc("CountPerformance"), false);
	Settings->SetBool(xc("PreventFpsDrop"), false);
	Settings->SetBool(xc("UseDebugConsole"), false);
	Settings->SetBool(xc("DrawFeaturesStatus"), false);
	Settings->SetBool(xc("DrawDebug"), false);

	Settings->SetBool(xc("ESP"), false);
	Settings->SetBool(xc("Skeletons"), false);
	Settings->SetBool(xc("2DBoxes"), false);
	Settings->SetBool(xc("LinesFromCenterToHead"), false);
	Settings->SetBool(xc("SkeletonsWithNicknames"), false);
	Settings->SetBool(xc("SkeletonsWithBoneLines"), false);
	Settings->SetBool(xc("SkeletonsWithDistance"), false);
	Settings->SetBool(xc("SkeletonsWithKills"), false);
	Settings->SetBool(xc("SkeletonsWithHealth"), false);
	Settings->SetBool(xc("SkeletonsWithBoneNumbers"), false);
	Settings->SetBool(xc("SkeletonsWithRadiusCircles"), false);

	Settings->SetBool(xc("DrawRadius"), false);
	Settings->SetBool(xc("YouAreVisibleAlarm"), false);

	Settings->SetBool(xc("Aimbot"), true);
	Settings->SetBool(xc("AutoAim"), true);
	
	Settings->SetBool(xc("SilentAim"), false);
	
	Settings->SetBool(xc("TriggerBot"), true);
	Settings->SetBool(xc("AutoTrigger"), false);
	
	Settings->SetInt(xc("AimbotSearchType"), (int)AimSearchType::ClosestByDistance);
	Settings->SetInt(xc("SilentAimSearchType"), (int)AimSearchType::ClosestToCrosshair);
	Settings->SetInt(xc("AimKey"), VK_LSHIFT);
	Settings->SetInt(xc("AimKeyONOFF"), VK_NUMLOCK);
	Settings->SetInt(xc("TriggerKey"), VK_RBUTTON);
	Settings->SetInt(xc("TriggerKeyONOFF"), VK_PAUSE);
	Settings->SetInt(xc("SilentAimONOFF"), VK_SCROLL);
	Settings->SetInt(xc("AimbotRadius"), 20);
	Settings->SetInt(xc("AimbotBone"), -1);
	Settings->SetInt(xc("TriggerBotRadius"), 8);
	Settings->SetInt(xc("TriggerBotBone"), -1);
	Settings->SetInt(xc("SilentAimRadius"), 10);
	Settings->SetInt(xc("SilentAimBone"), -1);
}