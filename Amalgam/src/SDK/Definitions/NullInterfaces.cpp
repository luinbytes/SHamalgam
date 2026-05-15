#include "Interfaces.h"

#include "../../Core/Core.h"

#define Validate(x) if (!x) { U::Core.AppendFailText("CNullInterfaces::Initialize() failed to initialize "#x); m_bFailed = true; }
#define ValidateNonLethal(x) if (!x) { const char* sMessage = "CNullInterfaces::Initialize() failed to initialize "#x; MessageBox(nullptr, sMessage, "Warning", MB_OK | MB_ICONERROR); U::Core.AppendFailText(sMessage); }

MAKE_SIGNATURE(Get_TFPartyClient, "client.dll", "48 8B 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56", 0x0);
#ifndef __linux__
MAKE_SIGNATURE(Get_SteamNetworkingUtils, "client.dll", "40 53 48 83 EC ? 48 8B D9 48 8D 15 ? ? ? ? 33 C9 FF 15 ? ? ? ? 33 C9", 0x0);
#endif

#ifdef __linux__
template <typename T>
T* LinuxModuleOffset(const char* sModule, uintptr_t uOffset)
{
	if (const auto hModule = GetModuleHandle(sModule))
		return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(hModule) + uOffset);
	return nullptr;
}

class CLinuxUniformRandomStream final : public IUniformRandomStream
{
public:
	void SetSeed(int iSeed) override
	{
		static auto RandomSeed = U::Memory.GetModuleExport<void(*)(int)>("vstdlib.dll", "RandomSeed");
		if (RandomSeed)
			RandomSeed(iSeed);
	}

	float RandomFloat(float flMinVal = 0.f, float flMaxVal = 1.f) override
	{
		static auto RandomFloat = U::Memory.GetModuleExport<float(*)(float, float)>("vstdlib.dll", "RandomFloat");
		return RandomFloat ? RandomFloat(flMinVal, flMaxVal) : flMinVal;
	}

	int RandomInt(int iMinVal, int iMaxVal) override
	{
		static auto RandomInt = U::Memory.GetModuleExport<int(*)(int, int)>("vstdlib.dll", "RandomInt");
		return RandomInt ? RandomInt(iMinVal, iMaxVal) : iMinVal;
	}

	float RandomFloatExp(float flMinVal = 0.f, float flMaxVal = 1.f, float flExponent = 1.f) override
	{
		static auto RandomFloatExp = U::Memory.GetModuleExport<float(*)(float, float, float)>("vstdlib.dll", "RandomFloatExp");
		return RandomFloatExp ? RandomFloatExp(flMinVal, flMaxVal, flExponent) : RandomFloat(flMinVal, flMaxVal);
	}
};
#endif

bool CNullInterfaces::Initialize()
{
#ifdef __linux__
	static CLinuxUniformRandomStream LinuxUniformRandomStream;
	I::UniformRandomStream = &LinuxUniformRandomStream;
	Validate(I::UniformRandomStream);

	if (auto pStaticPropMgrClient = U::Memory.FindInterface("engine.dll", "StaticPropMgrClient004"))
		I::StaticPropMgr = reinterpret_cast<CStaticPropMgr*>(reinterpret_cast<uintptr_t>(pStaticPropMgrClient) - sizeof(void*));
	Validate(I::StaticPropMgr);

	I::MoveHelper = LinuxModuleOffset<IMoveHelper>("client.dll", 0x2E58820);
	Validate(I::MoveHelper);

	I::ViewRenderBeams = LinuxModuleOffset<IViewRenderBeams>("client.dll", 0x2E65080);
	Validate(I::ViewRenderBeams);

	I::ViewRender = LinuxModuleOffset<IViewRender>("client.dll", 0x2E64200);
	Validate(I::ViewRender);

	I::Input = LinuxModuleOffset<IInput>("client.dll", 0x2EC5F80);
	Validate(I::Input);

	I::TFGCClientSystem = LinuxModuleOffset<CTFGCClientSystem>("client.dll", 0x2EF4160);
	Validate(I::TFGCClientSystem);
#endif

	I::TFPartyClient = S::Get_TFPartyClient.Call<CTFPartyClient*>();
	Validate(I::TFPartyClient);

	I::KeyValuesSystem = U::Memory.GetModuleExport<IKeyValuesSystem*(*)()>("vstdlib.dll", "KeyValuesSystem")();
	Validate(I::KeyValuesSystem);

	const HSteamPipe hsNewPipe = I::SteamClient->CreateSteamPipe();
	Validate(hsNewPipe);

	const HSteamPipe hsNewUser = I::SteamClient->ConnectToGlobalUser(hsNewPipe);
	Validate(hsNewUser);

	I::SteamFriends = I::SteamClient->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION);
	Validate(I::SteamFriends);

	I::SteamUtils = I::SteamClient->GetISteamUtils(hsNewUser, STEAMUTILS_INTERFACE_VERSION);
	Validate(I::SteamUtils);

	I::SteamApps = I::SteamClient->GetISteamApps(hsNewUser, hsNewPipe, STEAMAPPS_INTERFACE_VERSION);
	ValidateNonLethal(I::SteamApps);

	I::SteamUserStats = I::SteamClient->GetISteamUserStats(hsNewUser, hsNewPipe, STEAMUSERSTATS_INTERFACE_VERSION);
	Validate(I::SteamUserStats);

	I::SteamUser = I::SteamClient->GetISteamUser(hsNewUser, hsNewPipe, STEAMUSER_INTERFACE_VERSION);
	Validate(I::SteamUser);

#ifdef __linux__
	if (auto SteamNetworkingUtils = U::Memory.GetModuleExport<ISteamNetworkingUtils*(*)()>("libsteam_api.so", "SteamAPI_SteamNetworkingUtils_SteamAPI_v004"))
		I::SteamNetworkingUtils = SteamNetworkingUtils();
#else
	S::Get_SteamNetworkingUtils.Call<ISteamNetworkingUtils*>(&I::SteamNetworkingUtils);
#endif
	Validate(I::SteamNetworkingUtils);

	return !m_bFailed;
}
