#pragma once

struct Pattern {
	std::string Module;
	PBYTE Sequence;
	const char* Mask;
	int64_t Offset;
};

std::unordered_map<std::string, Pattern> patterns{
	{
		xc("IntersectSegment"), Pattern{
			xc("crossfire.exe"),
			(PBYTE)"\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x00\x4C\x8B\x44\x24\x00\x48\x8B\x54\x24\x00\x48\x8B\x0D\x00\x00\x00\x00\xFF\x50\x18\x48\x83\xC4\x28\xC3",
			"xxxx?xxxx?xxxxxxx????xxxxxxx?xxxx?xxx????xxxxxxxx",
			0x0
		}
	},
	{
		xc("GetNodeTransform"), Pattern{
			xc("crossfire.exe"),
			(PBYTE)"\x48\x8B\x44\x24\x00\x48\x89\x44\x24\x00\x44\x0F\xB6\x4C\x24\x00\x4C\x8B\x44\x24\x00\x8B\x54\x24\x50\x48\x8B\x4C\x24\x00\xE8",
			"xxxx?xxxx?xxxxx?xxxx?xxxxxxxx?x",
			-0x6C
		}
	},
	{
		xc("SetObjectDims"), Pattern{
			xc("crossfire.exe"),
			(PBYTE)"\x44\x89\x4C\x24\x00\x4C\x89\x44\x24\x00\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\xE8\x00\x00\x00\x00\x48\x83\xBC\x24\x00\x00\x00\x00\x00\x74\x0B\x48\x83\xBC\x24\x00\x00\x00\x00\x00\x75\x3E\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x3D\x00\x00\x00\x00\x00\x7C\x21",
			"xxxx?xxxx?xxxx?xxxx?xxx????xxxx?x????xxxx?????xxxxxx?????xxx????x????xx?????xx",
			0x0
		}
	},
	{
		xc("DoScreenshot"), Pattern{
			xc("CShell_x64.dll"),
			(PBYTE)"\x40\x53\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x84\x24\x00\x00\x00\x00\x66\xC7\x41\x00\x00\x00\x33\xC9\xFF\x15",
			"xxxxx????xxx????xxxxxxx????xxx???xxxx",
			0x0
		}
	}
};

std::unordered_map<std::string, DWORD64> addresses;

void InitAddresses() {
	for (const auto& p : patterns) {
		addresses[p.first] = findAddress(p.second.Module.c_str(), p.second.Sequence, p.second.Mask) + p.second.Offset;
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(p.first + ": " + NumberToHex(addresses[p.first]));
		}
	}
	addresses[xc("pLTClientShell")] = *(DWORD64*)((DWORD64)GetModuleHandleA(xc("CShell_x64.dll")) + (DWORD64)0x1E1A348);
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(std::string(xc("pLTClientShell")) + ": " + NumberToHex(addresses[xc("pLTClientShell")]));
	}
}