#include "Image.h"

void UI::Image::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
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
                    rect.Start.x,
                    rect.Start.y,
                    rect.End.x - rect.Start.x,
                    rect.End.y - rect.Start.y
                },
                { 0.0f, 0.0f },
                0.0f,
                WHITE);
}
