#pragma once

struct Pattern {
	std::string Module;
	const char* Sequence;
	int64_t Offset;
};

std::unordered_map<std::string, Pattern> patterns{
	{
		xc("IntersectSegment"), Pattern{
			xc("crossfire.exe"),
			"48 89 54 24 ? 48 89 4C 24 ? 48 83 EC 28 48 8B 05",
			0x0
		}
	},
	{
		xc("GetNodeTransform"), Pattern{
			xc("crossfire.exe"),
			"48 8B 44 24 ? 48 89 44 24 ? 44 0F B6 4C 24 ? 4C 8B 44 24 ? 8B",
			-0x6C
		}
	},
	{
		xc("SetObjectDims"), Pattern{
			xc("crossfire.exe"),
			"44 89 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 48 81 EC ? ? ? ? 48 8D",
			0x0
		}
	},
	{
		xc("DoScreenshot"), Pattern{
			xc("CShell_x64.dll"),
			"40 53 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 66 C7 41",
			0x0
		}
	},
	{
		xc("Detour28_3"), Pattern{
			xc("CShell_x64.dll"),
			"E8 ? ? ? ? 4C 8B C8 48 85 C0 0F 84 ? ? ? ? F3 0F 10 88 ? ? ? ? F3 0F 10 80",
			0x0
		}
	},
	{
		xc("SendToServer"), Pattern{
			xc("crossfire.exe"),
			"44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 48 81 EC ? ? ? ? 48 C7 84 24 ? ? ? ? ? ? ? ? 48 8B 84 24 ? ? ? ? 0F",
			0x0
		}
	}
};

uint64_t get_absolute_address(uint64_t instruction_addr, uint64_t instruction_size, long relative)
{
	return instruction_addr + relative + instruction_size;
}

uint64_t get_absolute_address(uint64_t instruction_addr)
{
	if (instruction_addr < 0x1000)
		return 0;

	uint16_t instruction_size = 6;
	uint16_t offset_to_relative = 2;

	auto opcode = (unsigned char*)(instruction_addr);

	if (opcode[0] >= 0x48 && opcode[0] <= 0x4F) // 64 bit operand
	{
		instruction_size++;
		offset_to_relative++;
	}
	else if (opcode[0] == 0x80) // cmp
	{
		instruction_size = 7;
		offset_to_relative = 2;
	}
	else if (opcode[0] == 0xE8) // call
	{
		instruction_size = 5;
		offset_to_relative = 1;
	}

	return get_absolute_address(instruction_addr, instruction_size, *reinterpret_cast<long*>(instruction_addr + offset_to_relative));
}

std::unordered_map<std::string, DWORD64> addresses;

void InitAddresses() {
	for (const auto& p : patterns) {
		auto modInfo = GetModuleInfo(p.second.Module.c_str());
		addresses[p.first] = (DWORD64)Sig::find(modInfo.first, modInfo.second, p.second.Sequence) + p.second.Offset;
		if (DebugConsole->IsAttached()) {
			DebugConsole->PrintMsg(p.first + ": " + NumberToHex(addresses[p.first]));
		}
	}
	auto cshInfo = GetModuleInfo(xc("CShell_x64.dll"));
	addresses[xc("pLTClientShell")] = *(DWORD64*)get_absolute_address((DWORD64)Sig::find(cshInfo.first, cshInfo.second, "48 8B 0D ? ? ? ? 0F B6 F2 33 DB 48 8D 54 24 ? 89 5C 24 30 48 8B 01 FF 90 ? ? ? ? 83 7C 24 ? ? 0F 8C ? ? ? ? 84 C0"));
	if (DebugConsole->IsAttached()) {
		DebugConsole->PrintMsg(std::string(xc("pLTClientShell")) + ": " + NumberToHex(addresses[xc("pLTClientShell")]));
	}
}