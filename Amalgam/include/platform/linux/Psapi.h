#pragma once

#include "Windows.h"
#include <cstddef>

struct MODULEINFO
{
	LPVOID lpBaseOfDll = nullptr;
	DWORD SizeOfImage = 0;
	LPVOID EntryPoint = nullptr;
};

BOOL GetModuleInformation(HANDLE, HMODULE hModule, MODULEINFO* info, DWORD cb);
DWORD GetModuleBaseName(HANDLE, HMODULE hModule, LPSTR name, DWORD size);

