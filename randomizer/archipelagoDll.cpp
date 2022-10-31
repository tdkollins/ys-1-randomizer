#include <Windows.h>
#include "ClientCore.h"

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientCore::Start, NULL, 0, NULL);
	}
	return TRUE;
}
