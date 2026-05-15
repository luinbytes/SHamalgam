#ifdef __linux__

#include <platform/linux/Psapi.h>
#include <dlfcn.h>
#include <link.h>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace
{
struct ModuleRecord
{
	std::string name;
	uintptr_t base = 0;
	size_t size = 0;
};

std::vector<ModuleRecord> EnumerateModules()
{
	std::vector<ModuleRecord> modules;
	dl_iterate_phdr([](dl_phdr_info* info, size_t, void* data) -> int
	{
		auto* out = static_cast<std::vector<ModuleRecord>*>(data);
		uintptr_t minAddr = UINTPTR_MAX;
		uintptr_t maxAddr = 0;

		for (int i = 0; i < info->dlpi_phnum; ++i)
		{
			const auto& ph = info->dlpi_phdr[i];
			if (ph.p_type != PT_LOAD)
				continue;

			uintptr_t start = info->dlpi_addr + ph.p_vaddr;
			uintptr_t end = start + ph.p_memsz;
			minAddr = std::min(minAddr, start);
			maxAddr = std::max(maxAddr, end);
		}

		if (minAddr != UINTPTR_MAX && maxAddr > minAddr)
		{
			ModuleRecord record;
			record.name = info->dlpi_name && *info->dlpi_name ? info->dlpi_name : "<main>";
			record.base = minAddr;
			record.size = maxAddr - minAddr;
			out->push_back(std::move(record));
		}

		return 0;
	}, &modules);
	return modules;
}

std::vector<std::string> ModuleAliases(const char* requested)
{
	if (!requested || !*requested)
		return {};

	std::string name = requested;
	std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	static const std::unordered_map<std::string, std::vector<std::string>> aliases =
	{
		{ "client.dll", { "client.so" } },
		{ "server.dll", { "server.so" } },
		{ "engine.dll", { "engine.so" } },
		{ "materialsystem.dll", { "materialsystem.so" } },
		{ "vguimatsurface.dll", { "vguimatsurface.so" } },
		{ "vgui2.dll", { "vgui2.so" } },
		{ "vphysics.dll", { "vphysics.so" } },
		{ "studiorender.dll", { "studiorender.so" } },
		{ "inputsystem.dll", { "inputsystem.so" } },
		{ "datacache.dll", { "datacache.so" } },
		{ "filesystem_stdio.dll", { "filesystem_stdio.so" } },
		{ "soundemittersystem.dll", { "soundemittersystem.so" } },
		{ "vstdlib.dll", { "libvstdlib.so", "vstdlib.so" } },
		{ "tier0.dll", { "libtier0.so", "tier0.so" } },
		{ "steam_api64.dll", { "libsteam_api.so", "steam_api.so" } },
		{ "steamclient64.dll", { "steamclient.so", "libsteamclient.so" } },
	};

	std::vector<std::string> result = { requested, name };
	if (auto found = aliases.find(name); found != aliases.end())
		result.insert(result.end(), found->second.begin(), found->second.end());
	else if (name.ends_with(".dll"))
		result.push_back(name.substr(0, name.size() - 4) + ".so");

	return result;
}

bool ModuleMatches(const ModuleRecord& module, const char* requested)
{
	if (!requested || !*requested)
		return module.name == "<main>";

	std::string name = module.name;
	auto slash = name.find_last_of('/');
	std::string base = slash == std::string::npos ? name : name.substr(slash + 1);
	std::transform(base.begin(), base.end(), base.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	for (auto alias : ModuleAliases(requested))
	{
		std::transform(alias.begin(), alias.end(), alias.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		if (base == alias || name.find(alias) != std::string::npos)
			return true;
	}

	return false;
}

const ModuleRecord* FindModuleByHandle(HMODULE handle, std::vector<ModuleRecord>& modules)
{
	uintptr_t address = reinterpret_cast<uintptr_t>(handle);
	for (const auto& module : modules)
	{
		if (address >= module.base && address < module.base + module.size)
			return &module;
		if (address == module.base)
			return &module;
	}
	return nullptr;
}
}

extern "C" HMODULE GetModuleHandle(const char* name)
{
	auto modules = EnumerateModules();
	for (const auto& module : modules)
	{
		if (ModuleMatches(module, name))
			return reinterpret_cast<HMODULE>(module.base);
	}

	for (const auto& alias : ModuleAliases(name))
	{
		if (auto handle = dlopen(alias.c_str(), RTLD_NOLOAD | RTLD_NOW))
			return handle;
	}

	return nullptr;
}

extern "C" FARPROC GetProcAddress(HMODULE hModule, const char* symbol)
{
	if (!symbol)
		return nullptr;

	if (auto proc = dlsym(hModule ? hModule : RTLD_DEFAULT, symbol))
		return proc;
	return dlsym(RTLD_DEFAULT, symbol);
}

extern "C" BOOL GetModuleHandleEx(DWORD flags, LPCSTR addressOrName, HMODULE* out)
{
	if (!out)
		return FALSE;

	if (flags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(addressOrName);
		auto modules = EnumerateModules();
		for (const auto& module : modules)
		{
			if (address >= module.base && address < module.base + module.size)
			{
				*out = reinterpret_cast<HMODULE>(module.base);
				return TRUE;
			}
		}
		return FALSE;
	}

	*out = GetModuleHandle(addressOrName);
	return *out ? TRUE : FALSE;
}

BOOL GetModuleInformation(HANDLE, HMODULE hModule, MODULEINFO* info, DWORD)
{
	if (!info || !hModule)
		return FALSE;

	auto modules = EnumerateModules();
	if (const auto* module = FindModuleByHandle(hModule, modules))
	{
		info->lpBaseOfDll = reinterpret_cast<LPVOID>(module->base);
		info->SizeOfImage = static_cast<DWORD>(module->size);
		info->EntryPoint = nullptr;
		return TRUE;
	}

	return FALSE;
}

DWORD GetModuleBaseName(HANDLE, HMODULE hModule, LPSTR name, DWORD size)
{
	if (!name || !size)
		return 0;

	auto modules = EnumerateModules();
	if (const auto* module = FindModuleByHandle(hModule, modules))
	{
		std::string base = module->name;
		auto slash = base.find_last_of('/');
		if (slash != std::string::npos)
			base = base.substr(slash + 1);

		std::strncpy(name, base.c_str(), size - 1);
		name[size - 1] = '\0';
		return static_cast<DWORD>(std::strlen(name));
	}

	return 0;
}

#endif
