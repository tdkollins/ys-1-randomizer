#include <Windows.h>

DWORD WINAPI MainThread(LPVOID param) {
	for ( ;; ) {
		if (GetAsyncKeyState(VK_F6) & 0x80000) {
			MessageBoxA(NULL, "Hello World", "Hello World", MB_OK);
		}
		Sleep(100);
	}
	return 0;
}

bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		MessageBoxA(NULL, "dll injected", "dll injected", MB_OK);
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	}
	return true;
}
