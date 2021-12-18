#pragma once
#include "globals.h"

namespace Console {
	void bypass() {
		DWORD t;
		VirtualProtect(FreeConsole, 5, PAGE_EXECUTE_READWRITE, &t);
		*(DWORD*)FreeConsole = 0xC3;
	}
	void open(LPCSTR title = "stellr") {
		AllocConsole();
		SetConsoleTitle(title);
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
	}
	void alwaysontop(HWND h = GetConsoleWindow()) {
		SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		ShowWindow(h, SW_NORMAL);
	}
	void clear() {
		// do this later
	}
}