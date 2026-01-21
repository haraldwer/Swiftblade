#include "BakedFunc.h"

#include "Shader.h"
#include "Interface/Textures.h"
#include "State/State.h"

void Rendering::BakedFunc::Bake(const Vec2I& InResolution, const std::function<Vec4F(const Vec2F&, const Vec2I&)>& InEvalFunc)
{
    CHECK_RETURN_LOG(!Unload(), "Failed to unload previous func");
    
    // Evaluate each coordinate
    // Maybe split workload using threads?
    
    int size = InResolution.x * InResolution.y;
    Color *pixels = static_cast<Color*>(malloc(size * sizeof(Color)));
    for (int y = 0; y < InResolution.y; y++)
    {
        float yPart = static_cast<float>(y) / static_cast<float>(InResolution.y);
        for (int x = 0; x < InResolution.x; x++)
        {
            float xPart = static_cast<float>(x) / static_cast<float>(InResolution.x);
            Vec4F val = InEvalFunc(Vec2F(xPart, yPart), { x, y });
            pixels[y * InResolution.x + x] = {
                static_cast<uint8>(val.x * 255),
                static_cast<uint8>(val.y * 255),
                static_cast<uint8>(val.z * 255),
                static_cast<uint8>(val.w * 255)
            };
        }
    }

    // TODO: Load texture from image
    /*
    Image image = {
        .data = pixels,
        .width = InResolution.x,
        .height = InResolution.y,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };

    texture = RHI::LoadTexture(pixels, InResolution)
    
    *tex = LoadTextureFromImage(image);
    UnloadImage(image);         // Unload CPU (RAM) image data (pixels)
    */
}

void Rendering::BakedFunc::Bind(const String &InName, ShaderResource &InShader, int &InOutSlot, TextureParamValue InFilter) const
{
    CHECK_RETURN(!baked);
    
    CHECK_ASSERT(!texture, "Tex nullptr");
    
    const int loc = InShader.GetLocation(InName);
    CHECK_RETURN(loc < 0);

    InOutSlot++;
    TextureCommand cmd;
    cmd.shaderLoc = loc;
    cmd.id = texture;
    cmd.filter = InFilter;
    rlState::current.Set(cmd, InOutSlot);
}

bool Rendering::BakedFunc::Unload()
{
    baked = false;
    if (texture != 0)
    {
        RHI::DestroyTexture(texture);
        texture = 0;
    }
    return true;
}
