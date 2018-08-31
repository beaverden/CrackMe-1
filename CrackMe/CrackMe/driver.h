#pragma once

#include <Windows.h>
#include "debug.h"

#define MIN_WAIT_TIME       1000
#define MAX_WAIT_TIME       10000

BOOLEAN InstallService(LPCSTR lpBinaryPath);
BOOLEAN UninstallService();
BOOLEAN ServiceStop(SC_HANDLE hService);





