#ifdef __linux__

#include "../Core/Core.h"
#include "../Utils/ExceptionHandler/ExceptionHandler.h"
#include <thread>
#include <atomic>

namespace
{
std::thread g_thread;
std::atomic_bool g_started = false;

void MainThread(void* handle)
{
	U::ExceptionHandler.Initialize(handle);
	U::Core.Load();
	U::Core.Loop();
	U::Core.Unload();
	U::ExceptionHandler.Unload();
}
}

__attribute__((constructor))
static void SHamalgamAttach()
{
	if (g_started.exchange(true))
		return;

	g_thread = std::thread(MainThread, nullptr);
	g_thread.detach();
}

__attribute__((destructor))
static void SHamalgamDetach()
{
	U::Core.m_bUnload = true;
}

#endif

