#include "Label.h"

#include "Core/Utility/RayUtility.h"

void UI::Label::Init()
{
    Element::Init();
    
    if (const auto fontRsc = font.Get())
    {
        if (const auto font = fontRsc->Get())
        {
            Vector2 size = MeasureTextEx(*font, text.c_str(), size, spacing); 
            cachedSize = { size.x, size.y };
        }
    }
}

void UI::Label::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
    
    const Vec2F startPos = rect.start;
    const Vec2F endPos = rect.end - cachedSize;
    const Vec2F pos {
        Utility::Math::Lerp(startPos.x, endPos.x, centering.x),
        Utility::Math::Lerp(startPos.y, endPos.y, centering.y)
    };
    const Vec2F viewPos = ReferenceToViewport(pos); 

    // Difference in size between reference and view
    const Rect view = ReferenceToViewport(rect); 
    const float orgSize = (rect.end - rect.start).Length();
    const float viewSize = (view.end - view.start).Length();
    const float sizeScale = viewSize / orgSize;
    
    const Vector2 origin = { 0.0f, 0.0f };
    const float rot = 0.0f;
    const Color tint = WHITE;
    if (const auto fontRsc = font.Get())
        if (const auto font = fontRsc->Get())
            DrawTextPro(
                *font,
                text.c_str(),
                { viewPos.x, viewPos.y},
                origin,
                rot,
                size * sizeScale,
                spacing * sizeScale,
                tint);
}

void UI::Label::SetText(const String& InText)
{
    text = InText;
    if (const auto fontRsc = font.Get())
    {
        if (const auto font = fontRsc->Get())
        {
            Vector2 size = MeasureTextEx(*font, text.c_str(), size, spacing); 
            cachedSize = { size.x, size.y };
        }
    }
    Invalidate(); 
}
