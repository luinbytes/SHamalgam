#ifdef __linux__

#include <MinHook/MinHook.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <unordered_map>

namespace
{
struct HookRecord
{
	void* target = nullptr;
	void* detour = nullptr;
	void* trampoline = nullptr;
	unsigned char original[14] = {};
	bool enabled = false;
};

std::unordered_map<void*, HookRecord> g_hooks;
bool g_initialized = false;

void WriteAbsoluteJump(unsigned char* dst, void* address)
{
	dst[0] = 0xFF;
	dst[1] = 0x25;
	std::memset(dst + 2, 0, 4);
	std::memcpy(dst + 6, &address, sizeof(void*));
}

bool ProtectWritable(void* address)
{
	const long pageSize = sysconf(_SC_PAGESIZE);
	uintptr_t page = reinterpret_cast<uintptr_t>(address) & ~(static_cast<uintptr_t>(pageSize) - 1);
	return mprotect(reinterpret_cast<void*>(page), static_cast<size_t>(pageSize), PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
}
}

extern "C" MH_STATUS MH_Initialize(void)
{
	if (g_initialized)
		return MH_ERROR_ALREADY_INITIALIZED;
	g_initialized = true;
	return MH_OK;
}

extern "C" MH_STATUS MH_Uninitialize(void)
{
	if (!g_initialized)
		return MH_ERROR_NOT_INITIALIZED;

	for (auto& [target, hook] : g_hooks)
		MH_DisableHook(target);

	for (auto& [_, hook] : g_hooks)
		std::free(hook.trampoline);

	g_hooks.clear();
	g_initialized = false;
	return MH_OK;
}

extern "C" MH_STATUS MH_CreateHook(void* pTarget, void* pDetour, void** ppOriginal)
{
	if (!g_initialized)
		return MH_ERROR_NOT_INITIALIZED;
	if (!pTarget || !pDetour)
		return MH_ERROR_UNSUPPORTED_FUNCTION;
	if (g_hooks.contains(pTarget))
		return MH_ERROR_ALREADY_CREATED;

	HookRecord hook;
	hook.target = pTarget;
	hook.detour = pDetour;
	std::memcpy(hook.original, pTarget, sizeof(hook.original));

	auto* trampoline = static_cast<unsigned char*>(std::aligned_alloc(16, 32));
	if (!trampoline)
		return MH_ERROR_MEMORY_ALLOC;

	std::memcpy(trampoline, hook.original, sizeof(hook.original));
	WriteAbsoluteJump(trampoline + sizeof(hook.original), static_cast<unsigned char*>(pTarget) + sizeof(hook.original));
	if (!ProtectWritable(trampoline))
	{
		std::free(trampoline);
		return MH_ERROR_MEMORY_PROTECT;
	}

	hook.trampoline = trampoline;
	if (ppOriginal)
		*ppOriginal = trampoline;

	g_hooks[pTarget] = hook;
	return MH_OK;
}

extern "C" MH_STATUS MH_RemoveHook(void* pTarget)
{
	if (!g_initialized)
		return MH_ERROR_NOT_INITIALIZED;

	auto it = g_hooks.find(pTarget);
	if (it == g_hooks.end())
		return MH_ERROR_NOT_CREATED;

	MH_DisableHook(pTarget);
	std::free(it->second.trampoline);
	g_hooks.erase(it);
	return MH_OK;
}

extern "C" MH_STATUS MH_EnableHook(void* pTarget)
{
	if (!g_initialized)
		return MH_ERROR_NOT_INITIALIZED;

	if (pTarget == MH_ALL_HOOKS)
	{
		for (auto& [target, _] : g_hooks)
			MH_EnableHook(target);
		return MH_OK;
	}

	auto it = g_hooks.find(pTarget);
	if (it == g_hooks.end())
		return MH_ERROR_NOT_CREATED;

	auto& hook = it->second;
	if (hook.enabled)
		return MH_ERROR_ENABLED;
	if (!ProtectWritable(hook.target))
		return MH_ERROR_MEMORY_PROTECT;

	WriteAbsoluteJump(static_cast<unsigned char*>(hook.target), hook.detour);
	__builtin___clear_cache(static_cast<char*>(hook.target), static_cast<char*>(hook.target) + sizeof(hook.original));
	hook.enabled = true;
	return MH_OK;
}

extern "C" MH_STATUS MH_DisableHook(void* pTarget)
{
	if (!g_initialized)
		return MH_ERROR_NOT_INITIALIZED;

	if (pTarget == MH_ALL_HOOKS)
	{
		for (auto& [target, _] : g_hooks)
			MH_DisableHook(target);
		return MH_OK;
	}

	auto it = g_hooks.find(pTarget);
	if (it == g_hooks.end())
		return MH_ERROR_NOT_CREATED;

	auto& hook = it->second;
	if (!hook.enabled)
		return MH_ERROR_DISABLED;
	if (!ProtectWritable(hook.target))
		return MH_ERROR_MEMORY_PROTECT;

	std::memcpy(hook.target, hook.original, sizeof(hook.original));
	__builtin___clear_cache(static_cast<char*>(hook.target), static_cast<char*>(hook.target) + sizeof(hook.original));
	hook.enabled = false;
	return MH_OK;
}

extern "C" MH_STATUS MH_QueueEnableHook(void* pTarget)
{
	return MH_EnableHook(pTarget);
}

extern "C" MH_STATUS MH_QueueDisableHook(void* pTarget)
{
	return MH_DisableHook(pTarget);
}

extern "C" MH_STATUS MH_ApplyQueued(void)
{
	return MH_OK;
}

extern "C" const char* MH_StatusToString(MH_STATUS status)
{
	switch (status)
	{
	case MH_OK: return "MH_OK";
	case MH_ERROR_ALREADY_INITIALIZED: return "MH_ERROR_ALREADY_INITIALIZED";
	case MH_ERROR_NOT_INITIALIZED: return "MH_ERROR_NOT_INITIALIZED";
	case MH_ERROR_ALREADY_CREATED: return "MH_ERROR_ALREADY_CREATED";
	case MH_ERROR_NOT_CREATED: return "MH_ERROR_NOT_CREATED";
	case MH_ERROR_ENABLED: return "MH_ERROR_ENABLED";
	case MH_ERROR_DISABLED: return "MH_ERROR_DISABLED";
	case MH_ERROR_NOT_EXECUTABLE: return "MH_ERROR_NOT_EXECUTABLE";
	case MH_ERROR_UNSUPPORTED_FUNCTION: return "MH_ERROR_UNSUPPORTED_FUNCTION";
	case MH_ERROR_MEMORY_ALLOC: return "MH_ERROR_MEMORY_ALLOC";
	case MH_ERROR_MEMORY_PROTECT: return "MH_ERROR_MEMORY_PROTECT";
	case MH_ERROR_MODULE_NOT_FOUND: return "MH_ERROR_MODULE_NOT_FOUND";
	case MH_ERROR_FUNCTION_NOT_FOUND: return "MH_ERROR_FUNCTION_NOT_FOUND";
	default: return "MH_UNKNOWN";
	}
}

#endif

