#include "ClientCore.h"
#include "GameHook.h"

VOID ClientCore::Start() {
	ClientCore *clientCore = new ClientCore();

	for ( ;; ) {
		clientCore->runLoop();
		Sleep(2000);
	}

	delete clientCore;
}

ClientCore::ClientCore() {
	ys = new GameHook();
}

ClientCore::~ClientCore() {
	delete ys;
}

VOID ClientCore::runLoop() {
}
