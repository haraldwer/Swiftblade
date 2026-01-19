#include "LuminTargets.h"
#include "raylib.h"

void Rendering::LuminTargets::Init(const RenderTexture &InTarget, const float InIrrScale, const float InRScale)
{
    const Vec3I res = {
        InTarget.texture.width,
        InTarget.texture.height,
        0
    };
    const Vec3F resf = res.To<float>();
    constexpr uint8 format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    irradianceTarget.Setup((resf * InIrrScale).To<int>(), "TexIrradiance", format);
    radianceTarget.Setup((resf * InRScale).To<int>(), "TexRadiance", format);
    irradianceFrameTarget.Setup(res, "TexIrradianceFrame", format);
    radianceFrameTarget.Setup(res, "TexRadianceFrame", format);
}


void Rendering::LuminTargets::Deinit()
{
    irradianceTarget.Unload();
    radianceTarget.Unload();
    irradianceFrameTarget.Unload();
    radianceFrameTarget.Unload();
}
