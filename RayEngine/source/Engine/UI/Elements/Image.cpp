#include "Image.h"

#include "raylib.h"

void UI::Image::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
    const Rect view = ReferenceToViewport(rect);
    if (const auto textureRes = texture.Get())
        if (const auto texture = textureRes->Get())
            DrawTexturePro(
                *texture,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(texture->width),
                    static_cast<float>(texture->height)
                }, {
                    view.start.x,
                    view.start.y,
                    view.end.x - view.start.x,
                    view.end.y - view.start.y
                },
                { 0.0f, 0.0f },
                Transform.rotation,
                WHITE);
}
