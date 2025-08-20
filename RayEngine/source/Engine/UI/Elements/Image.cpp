#include "Image.h"

#include "raylib.h"

void UI::Image::Draw(Container& InOwner)
{
    CHECK_RETURN(!visible);
    Element::Draw(InOwner);
    const Rect view = ReferenceToViewport(GetRect());
    if (const auto textureRes = texture.Get())
    {
        if (const auto tex = textureRes->Get())
        {
            Vec2F destSize = {
                view.end.x - view.start.x,
                view.end.y - view.start.y,
            };
            DrawTexturePro(
                *tex,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(tex->width),
                    static_cast<float>(tex->height)
                }, {
                    view.start.x + transform.pivot.x * destSize.x,
                    view.start.y + transform.pivot.y * destSize.y,
                    destSize.x,
                    destSize.y
                }, {
                    transform.pivot.x * destSize.x,
                    transform.pivot.y * destSize.y,
                },
                transform.rotation,
                {
                    static_cast<uint8>(imageColor.r * 255),
                    static_cast<uint8>(imageColor.g * 255),
                    static_cast<uint8>(imageColor.b * 255),
                    static_cast<uint8>(imageColor.a * 255)
                });
        }
    }
}

Vec2F UI::Image::GetDesiredSize() const
{
    if (transform.size == Vec2F::Zero())
    {
        if (const auto textureRes = texture.Get())
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
