#include "../SDK/SDK.h"

#ifdef __linux__
MAKE_SIGNATURE(CViewRender_DrawUnderwaterOverlay, "client.dll", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 83 EC ? 4C 8B A7 ? ? ? ? 4D 85 E4 0F 84 ? ? ? ? 48 8B 1D", 0x0);
#else
MAKE_SIGNATURE(CViewRender_DrawUnderwaterOverlay, "client.dll", "4C 8B DC 41 56 48 81 EC ? ? ? ? 4C 8B B1", 0x0);
#endif

MAKE_HOOK(CViewRender_DrawUnderwaterOverlay, S::CViewRender_DrawUnderwaterOverlay(), void,
	void* rcx)
{
	DEBUG_RETURN(CViewRender_DrawUnderwaterOverlay, rcx);

	if (!Vars::Visuals::Removals::ScreenOverlays.Value || SDK::CleanScreenshot())
		CALL_ORIGINAL(rcx);
}
