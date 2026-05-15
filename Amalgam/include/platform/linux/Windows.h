#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
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
using HKL = void*;
using HGLOBAL = void*;
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

#define CF_TEXT 1
#define GMEM_DDESHARE 0x2000

#define PAGE_EXECUTE_READWRITE 0x40
#define PROCESS_QUERY_INFORMATION 0x0400
#define PROCESS_VM_READ 0x0010
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x00000004
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT 0x00000002

#define VK_TAB 0x09
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_MENU 0x12
#define VK_PAUSE 0x13
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_A 0x41
#define VK_Z 0x5A
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_APPS 0x5D
#define VK_DIVIDE 0x6F
#define VK_F3 0x72
#define VK_F11 0x7A
#define VK_F13 0x7C
#define VK_F14 0x7D
#define VK_F15 0x7E
#define VK_F16 0x7F
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86
#define VK_F24 0x87
#define VK_NUMLOCK 0x90
#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU 0xA4
#define VK_RMENU 0xA5
#define VK_VOLUME_MUTE 0xAD
#define VK_VOLUME_DOWN 0xAE
#define VK_VOLUME_UP 0xAF
#define VK_MEDIA_NEXT_TRACK 0xB0
#define VK_MEDIA_PREV_TRACK 0xB1
#define VK_MEDIA_STOP 0xB2
#define VK_MEDIA_PLAY_PAUSE 0xB3
#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06

#define MAPVK_VK_TO_VSC 0

#define WM_KEYFIRST 0x0100
#define WM_KEYLAST 0x0109
#define WM_MOUSEFIRST 0x0200
#define WM_MOUSELAST 0x020D
#define GWLP_WNDPROC (-4)
#define SW_SHOWNORMAL 1

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

inline void OutputDebugString(const char* s)
{
	OutputDebugStringA(s);
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

inline HKL GetKeyboardLayout(DWORD)
{
	return nullptr;
}

inline HWND GetForegroundWindow()
{
	return nullptr;
}

inline unsigned int MapVirtualKeyEx(unsigned int code, unsigned int, HKL)
{
	return code;
}

inline int ToUnicodeEx(unsigned int, unsigned int, const BYTE*, wchar_t*, int, unsigned int, HKL)
{
	return 0;
}

inline int GetKeyNameText(LONG, char* buffer, int size)
{
	if (buffer && size > 0)
		*buffer = '\0';
	return 0;
}

inline DWORD GetWindowThreadProcessId(HWND, DWORD* processId)
{
	if (processId)
		*processId = GetCurrentProcessId();
	return 0;
}

inline int GetWindowText(HWND, char* text, int maxCount)
{
	if (text && maxCount > 0)
		*text = '\0';
	return 0;
}

inline BOOL EnumWindows(BOOL (CALLBACK*)(HWND, LPARAM), LPARAM)
{
	return TRUE;
}

inline BOOL OpenClipboard(HWND)
{
	return TRUE;
}

inline BOOL EmptyClipboard()
{
	return TRUE;
}

inline HGLOBAL GlobalAlloc(UINT, size_t bytes)
{
	return std::malloc(bytes);
}

inline void* GlobalLock(HGLOBAL memory)
{
	return memory;
}

inline BOOL GlobalUnlock(HGLOBAL)
{
	return TRUE;
}

inline std::string& ClipboardText()
{
	static std::string s_clipboard;
	return s_clipboard;
}

inline HANDLE SetClipboardData(UINT, HANDLE data)
{
	ClipboardText() = data ? static_cast<const char*>(data) : "";
	return data;
}

inline HANDLE GetClipboardData(UINT)
{
	return ClipboardText().empty() ? nullptr : const_cast<char*>(ClipboardText().c_str());
}

inline BOOL CloseClipboard()
{
	return TRUE;
}

inline void* ShellExecuteA(HWND, const char*, const char* file, const char*, const char*, int)
{
	if (!file || !*file)
		return nullptr;

	const pid_t pid = fork();
	if (pid == 0)
	{
		execlp("xdg-open", "xdg-open", file, static_cast<char*>(nullptr));
		_exit(127);
	}

	return pid < 0 ? nullptr : reinterpret_cast<void*>(1);
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
