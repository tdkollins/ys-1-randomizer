#include "GameHook.h"

GameHook::GameHook() {
	initialize();
}

VOID GameHook::initialize() {
	MessageBoxA(NULL, "Game hook initalized", "Game hook intialized", MB_OK);
}
