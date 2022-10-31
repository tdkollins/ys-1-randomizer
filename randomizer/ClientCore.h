#ifndef CLIENTCORE_H
#define CLIENTCORE_H

#include <Windows.h>
#include "GameHook.h"

class ClientCore {
	GameHook *ys;
	ClientCore();
	VOID runLoop();
public:
	static VOID Start();
	~ClientCore();
};

#endif
