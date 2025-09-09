#include "Image.h"

#include "raylib.h"

void UI::Image::Draw(Container& InOwner)
{
    CHECK_RETURN(!visible);
    Element::Draw(InOwner);
    
    const auto r = cachedDrawRect;
    if (const auto textureRes = properties.texture.Get())
    {
        if (const auto tex = textureRes->Get())
        {
            DrawTexturePro(
                *tex,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(tex->width),
                    static_cast<float>(tex->height)
                }, {
                    r.x + transform.pivot.x * r.z,
                    r.y + transform.pivot.y * r.w,
                    r.z,
                    r.w
                }, {
                    transform.pivot.x * r.z,
                    transform.pivot.y * r.w,
                },
                transform.rotation,
                {
                    static_cast<uint8>(properties.color.r * 255),
                    static_cast<uint8>(properties.color.g * 255),
                    static_cast<uint8>(properties.color.b * 255),
                    static_cast<uint8>(properties.color.a * 255)
                });
        }
    }
}

Vec2F UI::Image::GetDesiredSize() const
{
    if (transform.size == Vec2F::Zero())
    {
        if (const auto textureRes = properties.texture.Get())
        {
            if (const auto tex = textureRes->Get())
            {
                Vec2F padding = {
                    transform.padding.horizontal.x + transform.padding.horizontal.y, 
                    transform.padding.vertical.x + transform.padding.vertical.y
                };
                Vec2F s = {
                    static_cast<float>(tex->height),
                    static_cast<float>(tex->width)
                };
                return s + padding;
            }
        }
    }
    
    return Element::GetDesiredSize();
}
