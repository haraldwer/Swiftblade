#include "Element.h"

#include "Container.h"
#include "Engine/Rendering/Renderer.h"

void UI::Element::Init()
{
}

void UI::Element::Update()
{
}

void UI::Element::Draw()
{
    DrawRect(CachedRect); 
}

void UI::Element::Invalidate()
{
    if (Parent)
        Parent->Invalidate();
}

void UI::Element::RefreshRect(const Rect& InContainer)
{
    CachedRect = CalculateRect(InContainer);
}

bool UI::Element::IsHovered() const
{
    const Vector2 mp = GetMousePosition();
    const Rect screen = ToScreen(CachedRect); 
    return
        mp.x > screen.Start.x &&
        mp.x < screen.End.x &&
        mp.y > screen.Start.y &&
        mp.y < screen.End.y;
}

bool UI::Element::IsClicked() const
{
    return IsHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

UI::Rect UI::Element::GetReferenceRect()
{
    const Vec2F res = Rendering::Renderer::Get().GetResolution();
    const float aspect = res.x / res.y; 
    return {
        Vec2F::Zero(),
        {
            1000.0f * aspect,
            1000.0f
        }
    };
}

UI::Rect UI::Element::ToScreen(const Rect& InRect)
{
    return { ToScreen(InRect.Start), ToScreen(InRect.End) };    
}

Vec2F UI::Element::ToScreen(const Vec2F& InVec)
{
    // Convert rect from reference space to screen relative
    // First, convert to 0 - 1 space
    // Then, convert to 0 - virtual size
    // TODO: Also consider ref start
    // TODO: Fix stretching
    
    const Vec2F res = Rendering::Renderer::Get().GetResolution();
    const Rect ref = GetReferenceRect();
    return {
        (InVec / ref.End) * res,
    };
}

UI::Rect UI::Element::CalculateRect(const Rect& InContainer) const
{
    // Position - The actual position information
    // Size - The actual size information
    // Alignment - How the position and size should be used relative to parent
    // Padding - Padding around the current element
    // Anchor - Reference point on InContainer
    // Pivot - Size point relative to position
    // Margin - Margin inside the current element, used by container for child elements

    // Set up parent container
    Rect parent = InContainer;
    parent.Start.x += Transform.Padding.Horizontal.x;
    parent.End.x -= Transform.Padding.Horizontal.y;
    parent.Start.y += Transform.Padding.Vertical.x;
    parent.End.y -= Transform.Padding.Vertical.y;

    // Calculate anchor point
    const Vec2F anchor = {
        LERP(parent.Start.x, parent.End.x, Transform.Anchor.x),
        LERP(parent.Start.y, parent.End.y, Transform.Anchor.y)
    };
    
    // Calculate position and size
    Rect absolute;
    absolute.Start = anchor + Transform.Position + Transform.Size * Transform.Pivot;
    absolute.End = anchor + Transform.Position + Transform.Size * (Vec2F(1.0f) - Transform.Pivot);
    
    // And blend to container using alignment
    Rect result;
    result.Start.x = LERP(absolute.Start.x, parent.Start.x, Transform.Alignment.Horizontal.x);
    result.Start.y = LERP(absolute.Start.y, parent.Start.y, Transform.Alignment.Vertical.x);
    result.End.x = LERP(absolute.End.x, parent.End.x, Transform.Alignment.Horizontal.y);
    result.End.y = LERP(absolute.End.y, parent.End.y, Transform.Alignment.Vertical.y);
    
    return result; 
}

void UI::Element::DrawRect(const Rect& InRect)
{
    return; 
    const Rect screenRect = ToScreen(InRect);
    const Vec2F size = screenRect.End - screenRect.Start;
    const Vec2F pos = screenRect.Start;
    DrawRectangleLines(
        static_cast<int>(pos.x + 0.5f),
        static_cast<int>(pos.y + 0.5f),
        static_cast<int>(size.x + 0.5f),    
        static_cast<int>(size.y + 0.5f),
        RED);
}
