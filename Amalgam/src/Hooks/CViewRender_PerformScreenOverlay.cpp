#include "../SDK/SDK.h"

#ifdef __linux__
MAKE_SIGNATURE(CViewRender_PerformScreenOverlay, "client.dll", "55 31 C0 48 89 E5 41 57 41 56 41 55 45 89 C5 41 54 53 48 89 FB 48 83 EC ? 4C 8B 25", 0x0);
#else
MAKE_SIGNATURE(CViewRender_PerformScreenOverlay, "client.dll", "4C 8B DC 49 89 5B ? 89 54 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC", 0x0);
#endif

MAKE_HOOK(CViewRender_PerformScreenOverlay, S::CViewRender_PerformScreenOverlay(), void,
	void* rcx, int x, int y, int w, int h)
{
	DEBUG_RETURN(CViewRender_PerformScreenOverlay, rcx, x, y, w, h);

	if (!Vars::Visuals::Removals::ScreenOverlays.Value || SDK::CleanScreenshot())
		CALL_ORIGINAL(rcx, x, y, w, h);
}
