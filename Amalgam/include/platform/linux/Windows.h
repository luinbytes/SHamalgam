#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/mman.h>

using BOOL = int;
using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned long;
using DWORD64 = unsigned long long;
using DWORD_PTR = uintptr_t;
using UINT = unsigned int;
using ULONG = unsigned long;
using LONG = long;
using LONG_PTR = intptr_t;
using LPARAM = intptr_t;
using WPARAM = uintptr_t;
using LRESULT = intptr_t;
using HRESULT = long;
using HANDLE = void*;
using HMODULE = void*;
using HINSTANCE = void*;
using HWND = void*;
using HMONITOR = void*;
using HCURSOR = unsigned long;
using HCursor = unsigned long;
using LPVOID = void*;
using LPCVOID = const void*;
using LPCSTR = const char*;
using LPSTR = char*;
using PVOID = void*;
using VOID = void;
using FARPROC = void*;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define WINAPI
#define CALLBACK
#define __stdcall

#ifndef __fastcall
#define __fastcall
#endif

#ifndef __cdecl
#define __cdecl
#endif

#define MAX_PATH 4096
#define DLL_PROCESS_ATTACH 1
#define EXIT_SUCCESS 0

#define MB_OK 0
#define MB_ICONERROR 0

#define PAGE_EXECUTE_READWRITE 0x40
#define PROCESS_QUERY_INFORMATION 0x0400
#define PROCESS_VM_READ 0x0010
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x00000004
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT 0x00000002

#define VK_SHIFT 0x10
#define VK_RETURN 0x0D
#define VK_MENU 0x12
#define VK_INSERT 0x2D
#define VK_F3 0x72
#define VK_F11 0x7A
#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04

#define WM_KEYFIRST 0x0100
#define WM_KEYLAST 0x0109
#define WM_MOUSEFIRST 0x0200
#define WM_MOUSELAST 0x020D
#define GWLP_WNDPROC (-4)

struct RECT
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
};

struct RGNDATA {};

using WNDPROC = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);

extern "C" HMODULE GetModuleHandle(const char* name);
extern "C" FARPROC GetProcAddress(HMODULE hModule, const char* symbol);
extern "C" BOOL GetModuleHandleEx(DWORD flags, LPCSTR addressOrName, HMODULE* out);

inline void Sleep(DWORD ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline DWORD GetCurrentProcessId()
{
	return static_cast<DWORD>(getpid());
}

inline HANDLE GetCurrentProcess()
{
	return reinterpret_cast<HANDLE>(-1);
}

inline HANDLE OpenProcess(DWORD, BOOL, DWORD)
{
	return GetCurrentProcess();
}

inline BOOL CloseHandle(HANDLE)
{
	return TRUE;
}

inline void OutputDebugStringA(const char* s)
{
	if (s)
		write(STDERR_FILENO, s, std::strlen(s));
}

inline int MessageBox(HWND, const char* text, const char* title, UINT)
{
	if (title)
		OutputDebugStringA(title);
	OutputDebugStringA(": ");
	if (text)
		OutputDebugStringA(text);
	OutputDebugStringA("\n");
	return 0;
}

inline short GetAsyncKeyState(int)
{
	return 0;
}

inline BOOL VirtualProtect(LPVOID address, size_t size, DWORD, DWORD* oldProtect)
{
	if (oldProtect)
		*oldProtect = PAGE_EXECUTE_READWRITE;

	const long pageSize = sysconf(_SC_PAGESIZE);
	uintptr_t start = reinterpret_cast<uintptr_t>(address) & ~(static_cast<uintptr_t>(pageSize) - 1);
	uintptr_t end = (reinterpret_cast<uintptr_t>(address) + size + pageSize - 1) & ~(static_cast<uintptr_t>(pageSize) - 1);
	return mprotect(reinterpret_cast<void*>(start), end - start, PROT_READ | PROT_WRITE | PROT_EXEC) == 0 ? TRUE : FALSE;
}

inline LONG_PTR SetWindowLongPtr(HWND, int, LONG_PTR)
{
	return 0;
}

inline LRESULT CallWindowProc(WNDPROC proc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return proc ? proc(hWnd, msg, wParam, lParam) : 0;
}

inline void FreeLibraryAndExitThread(HMODULE, DWORD code)
{
	std::exit(static_cast<int>(code));
}
