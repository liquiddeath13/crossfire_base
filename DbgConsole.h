#pragma once
#include <iostream>
#include <tchar.h>

struct DbgConsole {
private:
	bool Attached;
public:
	bool IsAttached() {
		return Attached;
	}
	void Attach() {
		if (!ac) {
			//err handling
			return;
		}

		FILE* fDummy;
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);
		std::cout.clear();
		std::clog.clear();
		std::cerr.clear();
		std::cin.clear();

		HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		sstdh(STD_OUTPUT_HANDLE, hConOut);
		sstdh(STD_ERROR_HANDLE, hConOut);
		sstdh(STD_INPUT_HANDLE, hConIn);
		std::wcout.clear();
		std::wclog.clear();
		std::wcerr.clear();
		std::wcin.clear();

		Attached = true;
	}
	void Detach() {
		fc;
		Attached = false;
	}
	void PrintMsg(std::string msg) {
		std::cout << msg << std::endl;
	}
};

DbgConsole* DebugConsole = new DbgConsole();