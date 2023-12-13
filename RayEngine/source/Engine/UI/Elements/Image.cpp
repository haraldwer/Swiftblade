#include "Image.h"

void UI::Image::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
    const Rect screenRect = ToScreen(rect);
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
                    screenRect.Start.x,
                    screenRect.Start.y,
                    screenRect.End.x - screenRect.Start.x,
                    screenRect.End.y - screenRect.Start.y
                },
                { 0.0f, 0.0f },
                0.0f,
                WHITE);
}
