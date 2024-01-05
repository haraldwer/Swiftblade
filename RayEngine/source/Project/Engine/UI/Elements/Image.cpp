#include "Image.h"

void UI::Image::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
    const Rect view = ToViewport(rect);
    if (const auto textureRes = Texture.Get())
        if (const auto texture = textureRes->Get())
            DrawTexturePro(
                *texture,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(texture->width),
                    static_cast<float>(texture->height)
                }, {
                    view.Start.x,
                    view.Start.y,
                    view.End.x - view.Start.x,
                    view.End.y - view.Start.y
                },
                { 0.0f, 0.0f },
                0.0f,
                WHITE);
}
