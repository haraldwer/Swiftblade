#include "Label.h"

#include "Utility/RayUtility.h"

void UI::Label::Init()
{
    Element::Init();
    
    if (const auto fontRsc = Font.Get())
    {
        if (const auto font = fontRsc->Get())
        {
            Vector2 size = MeasureTextEx(*font, Text.c_str(), Size, Spacing); 
            CachedSize = { size.x, size.y };
        }
    }
}

void UI::Label::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
    
    const Vec2F startPos = rect.Start;
    const Vec2F endPos = rect.End - CachedSize;
    const Vec2F pos {
        LERP(startPos.x, endPos.x, Centering.x),
        LERP(startPos.y, endPos.y, Centering.y)
    };
    const Vec2F screenPos = ToScreen(pos); 

    // Difference in size between rect and screenRect
    const Rect screenRect = ToScreen(rect); 
    const float orgSize = (rect.End - rect.Start).Length();
    const float screenSize = (screenRect.End - screenRect.Start).Length();
    const float sizeScale = screenSize / orgSize;
    
    const Vector2 origin = { 0.0f, 0.0f };
    const float rot = 0.0f;
    const Color tint = WHITE;
    if (const auto fontRsc = Font.Get())
        if (const auto font = fontRsc->Get())
            DrawTextPro(
                *font,
                Text.c_str(),
                { screenPos.x, screenPos.y},
                origin,
                rot,
                Size * sizeScale,
                Spacing * sizeScale,
                tint);
}

void UI::Label::SetText(const String& InText)
{
    Text = InText;
    if (const auto fontRsc = Font.Get())
    {
        if (const auto font = fontRsc->Get())
        {
            Vector2 size = MeasureTextEx(*font, Text.c_str(), Size, Spacing); 
            CachedSize = { size.x, size.y };
        }
    }
    Invalidate(); 
}
