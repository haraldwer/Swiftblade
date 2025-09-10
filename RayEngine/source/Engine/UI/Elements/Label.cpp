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

    Element::Draw(InOwner);
    
    const Rect rect = GetRect();
    const Vec2F startPos = rect.start;
    const Vec2F endPos = rect.end - cachedSize;
    const Vec2F pos = (startPos + endPos) * 0.5f;
    const Vec2F viewPos = ReferenceToViewport(pos); 

    // Difference in size between reference and view
    const Rect view = ReferenceToViewport(rect); 
    const float orgSize = (rect.end - rect.start).Length();
    const float viewSize = (view.end - view.start).Length();
    const float sizeScale = viewSize / orgSize;
    
    const Vector2 origin = { 0.0f, 0.0f };
    const float rot = 0.0f;
    const Color tint = {
        static_cast<uint8>(properties.color.r * 255),
        static_cast<uint8>(properties.color.g * 255),
        static_cast<uint8>(properties.color.b * 255),
        static_cast<uint8>(properties.color.a * 255),
    };

    float screenSize = GetScreenSize();
    const auto fontRsc = properties.font.Get();
    CHECK_RETURN(!fontRsc);
    const auto fontPtr = fontRsc->Get(static_cast<int>(screenSize)))
    CHECK_RETURN(!fontPtr);
    const auto shader = fontRsc->GetSDFShader();
    BeginShaderMode(*shader);
    DrawTextPro(
        *fontPtr,
        properties.text.c_str(),
        { viewPos.x, viewPos.y},
        origin,
        rot,
        screenSize,
        properties.spacing * sizeScale,
        tint);
    EndShaderMode();
}

Vec2F UI::Label::GetDesiredSize() const
{
    if (transform.size == Vec2F::Zero())
        return cachedSize;
    return transform.size;
}

void UI::Label::SetText(const String& InText)
{
    properties.text = InText;
    CacheSize();
    Invalidate(); 
}

void UI::Label::CacheSize()
{
    if (const auto fontRsc = properties.font.Get())
    {
        float screenSize = GetScreenSize();
        if (const auto fontPtr = fontRsc->Get(static_cast<int>(screenSize)))
        {
            Vector2 measure = MeasureTextEx(*fontPtr, properties.text.c_str(), screenSize, properties.spacing); 
            cachedSize = Vec2F(measure.x, measure.y) * properties.size / screenSize;
        }
    }
}

float UI::Label::GetScreenSize() const
{
    Vec2F s = { properties.size };
    Vec2F view = ReferenceToViewport(s);
    return Utility::Math::Min(view.x, view.y);
}
