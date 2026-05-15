#include "../SDK/SDK.h"

#ifdef __linux__
MAKE_SIGNATURE(CStaticPropMgr_DrawStaticProps, "engine.dll", "55 31 C0 48 89 E5 41 57 41 56 41 89 D6 41 55 49 89 F5 41 54 49 89 FC 53 89 CB 48 83 EC ? 4C 8B 3D", 0x0);
#else
MAKE_SIGNATURE(CStaticPropMgr_DrawStaticProps, "engine.dll", "4C 8B DC 49 89 5B ? 49 89 6B ? 49 89 73 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 4C 8B 3D", 0x0);
#endif

static bool s_bDrawingProps = false;

MAKE_HOOK(CStaticPropMgr_DrawStaticProps, S::CStaticPropMgr_DrawStaticProps(), void,
	void* rcx, IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe)
{
	DEBUG_RETURN(CStaticPropMgr_DrawStaticProps, rcx, pProps, count, bShadowDepth, drawVCollideWireframe);

	s_bDrawingProps = true;
	CALL_ORIGINAL(rcx, pProps, count, bShadowDepth, drawVCollideWireframe);
	s_bDrawingProps = false;
}

MAKE_HOOK(CStudioRender_SetColorModulation, U::Memory.GetVirtual(I::StudioRender, 27), void,
	void* rcx, const float* pColor)
{
	DEBUG_RETURN(CStudioRender_SetColorModulation, rcx, pColor);

	if (!s_bDrawingProps || !(Vars::Visuals::World::Modulations.Value & Vars::Visuals::World::ModulationsEnum::Prop) || SDK::CleanScreenshot())
		return CALL_ORIGINAL(rcx, pColor);

	float flColor[3] = {
		Vars::Colors::PropModulation.Value.r / 255.f,
		Vars::Colors::PropModulation.Value.g / 255.f,
		Vars::Colors::PropModulation.Value.b / 255.f
	};
	CALL_ORIGINAL(rcx, flColor);
}

MAKE_HOOK(CStudioRender_SetAlphaModulation, U::Memory.GetVirtual(I::StudioRender, 28), void,
	void* rcx, float flAlpha)
{
	DEBUG_RETURN(CStudioRender_SetAlphaModulation, rcx, flAlpha);

	if (!s_bDrawingProps || !(Vars::Visuals::World::Modulations.Value & Vars::Visuals::World::ModulationsEnum::Prop) || SDK::CleanScreenshot())
		return CALL_ORIGINAL(rcx, flAlpha);

	CALL_ORIGINAL(rcx, Vars::Colors::PropModulation.Value.a / 255.f * flAlpha);
}
