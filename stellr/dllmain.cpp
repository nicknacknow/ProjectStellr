#include "globals.h"

#include "TaskScheduler.h"

void init() {
	Console::bypass();
	Console::open();
	Console::alwaysontop();

	Hooks::hookHandler* hookHandler = Hooks::hookHandler::GetSingleton();
	RBX::TaskScheduler* taskScheduler = RBX::TaskScheduler::GetSingleton();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
	if (reason == DLL_PROCESS_ATTACH) return CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)init, 0, 0, 0));
	return TRUE;
}