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
        Utility::Math::Lerp(startPos.x, endPos.x, Centering.x),
        Utility::Math::Lerp(startPos.y, endPos.y, Centering.y)
    };
    const Vec2F viewPos = ReferenceToViewport(pos); 

    // Difference in size between reference and view
    const Rect view = ReferenceToViewport(rect); 
    const float orgSize = (rect.End - rect.Start).Length();
    const float viewSize = (view.End - view.Start).Length();
    const float sizeScale = viewSize / orgSize;
    
    const Vector2 origin = { 0.0f, 0.0f };
    const float rot = 0.0f;
    const Color tint = WHITE;
    if (const auto fontRsc = Font.Get())
        if (const auto font = fontRsc->Get())
            DrawTextPro(
                *font,
                Text.c_str(),
                { viewPos.x, viewPos.y},
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
