#pragma once

#include <Windows.h>

#define TH32CS_SNAPMODULE 0x00000008
#define TH32CS_SNAPMODULE32 0x00000010

struct MODULEENTRY32
{
	DWORD dwSize = sizeof(MODULEENTRY32);
	char szModule[MAX_PATH] = {};
	char szExePath[MAX_PATH] = {};
	BYTE* modBaseAddr = nullptr;
	DWORD modBaseSize = 0;
	HMODULE hModule = nullptr;
};

inline HANDLE CreateToolhelp32Snapshot(DWORD, DWORD)
{
	return nullptr;
}

inline BOOL Module32First(HANDLE, MODULEENTRY32*)
{
	return FALSE;
}

inline BOOL Module32Next(HANDLE, MODULEENTRY32*)
{
	return FALSE;
}

