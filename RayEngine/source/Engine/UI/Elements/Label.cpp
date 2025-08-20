#include "Label.h"

#include "Core/Utility/RayUtility.h"
#include "Rendering/Manager.h"

void UI::Label::Init(Container& InOwner)
{
    Element::Init(InOwner);
    CacheSize();
}

void UI::Label::Draw(Container& InOwner)
{
    CHECK_RETURN(!visible);

    if (Rendering::Manager::Get().GetConfig().DrawElementRects.Get())
        DrawRect(cachedRect);
    
    const Rect rect = GetRect();
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
    const Color tint = {
        static_cast<uint8>(color.r * 255),
        static_cast<uint8>(color.g * 255),
        static_cast<uint8>(color.b * 255),
        static_cast<uint8>(color.a * 255),
    };

    float screenSize = GetScreenSize();
    if (const auto fontRsc = font.Get())
        if (const auto fontPtr = fontRsc->Get(static_cast<int>(screenSize)))
            DrawTextPro(
                *fontPtr,
                text.c_str(),
                { viewPos.x, viewPos.y},
                origin,
                rot,
                screenSize,
                spacing * sizeScale,
                tint);
}

Vec2F UI::Label::GetDesiredSize() const
{
    Vec2F padding = {
        transform.padding.horizontal.x + transform.padding.horizontal.y, 
        transform.padding.vertical.x + transform.padding.vertical.y
    };
    if (transform.size == Vec2F::Zero())
        return cachedSize + padding;
    return transform.size + padding;
}

void UI::Label::SetText(const String& InText)
{
    text = InText;
    CacheSize();
    Invalidate(); 
}

void UI::Label::CacheSize()
{
    if (const auto fontRsc = font.Get())
    {
        float screenSize = GetScreenSize();
        if (const auto fontPtr = fontRsc->Get(static_cast<int>(screenSize)))
        {
            Vector2 measure = MeasureTextEx(*fontPtr, text.c_str(), screenSize, spacing); 
            cachedSize = Vec2F(measure.x, measure.y) * size / screenSize;
        }
    }
}

float UI::Label::GetScreenSize() const
{
    Vec2F s = { size };
    Vec2F view = ReferenceToViewport(s);
    return Utility::Math::Min(view.x, view.y);
}
