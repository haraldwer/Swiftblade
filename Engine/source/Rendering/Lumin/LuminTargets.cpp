#include "LuminTargets.h"

void Rendering::LuminTargets::Init(const Vec2I& InRes, const float InIrrScale, const float InRScale)
{
    const Vec3I res = { InRes.x, InRes.y, 0 };
    const Vec3F resf = res.To<float>();
    irradianceTarget.Setup((resf * InIrrScale).To<int>(), "TexIrradiance");
    radianceTarget.Setup((resf * InRScale).To<int>(), "TexRadiance");
    irradianceFrameTarget.Setup(res, "TexIrradianceFrame");
    radianceFrameTarget.Setup(res, "TexRadianceFrame");
}


void Rendering::LuminTargets::Deinit()
{
    irradianceTarget.Unload();
    radianceTarget.Unload();
    irradianceFrameTarget.Unload();
    radianceFrameTarget.Unload();
}
