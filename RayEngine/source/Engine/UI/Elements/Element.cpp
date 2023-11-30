#include "Element.h"

#include "Container.h"

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
    {
        Parent->Invalidate();
    }
    else
    {
        const Rect rootRect {
            Vec2F::Zero(),
            {
                static_cast<float>(GetRenderWidth()),
                static_cast<float>(GetRenderHeight())
            }
        };
        RefreshRect(rootRect);
    }
}

void UI::Element::RefreshRect(const Rect& InContainer)
{
    CachedRect = CalculateRect(InContainer);
}

bool UI::Element::IsHovered() const
{
    const Vector2 mp = GetMousePosition();
    return
        mp.x > CachedRect.Start.x &&
        mp.x < CachedRect.End.x &&
        mp.y > CachedRect.Start.y &&
        mp.y < CachedRect.End.y;
}

bool UI::Element::IsClicked() const
{
    return IsHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
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
    Vec2F size = InRect.End - InRect.Start;
    Vec2F pos = InRect.Start;
    DrawRectangleLines(
        static_cast<int>(pos.x),
        static_cast<int>(pos.y),
        static_cast<int>(size.x),
        static_cast<int>(size.y),
        RED);
}
