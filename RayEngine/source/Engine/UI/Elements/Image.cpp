#include "Image.h"

#include "raylib.h"

void UI::Image::Draw(Instance& InInstance)
{
    const Rect rect = GetRect();
    DrawRect(rect);
    const Rect view = ReferenceToViewport(rect);
    if (const auto textureRes = texture.Get())
        if (const auto tex = textureRes->Get())
            DrawTexturePro(
                *tex,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(tex->width),
                    static_cast<float>(tex->height)
                }, {
                    view.start.x,
                    view.start.y,
                    view.end.x - view.start.x,
                    view.end.y - view.start.y
                },
                { 0.0f, 0.0f },
                transform.rotation,
                WHITE);
}
