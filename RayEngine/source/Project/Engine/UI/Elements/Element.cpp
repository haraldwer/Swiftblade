#include "Element.h"

#include "Container.h"
#include "Engine/Input/Manager.h"
#include "Engine/Rendering/Manager.h"

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

void UI::Element::SetTransform(const UI::Transform& InTransform)
{
    Transform = InTransform;
}

bool UI::Element::IsHovered() const
{
    const Vector2 mp = GetMousePosition();
    const Vec2F relative = ScreenToViewport({ mp.x, mp.y });
    return
        relative.x > CachedRect.Start.x &&
        relative.x < CachedRect.End.x &&
        relative.y > CachedRect.Start.y &&
        relative.y < CachedRect.End.y;
}

bool UI::Element::IsClicked() const
{
    return IsHovered() && Input::Action::Get("LM").Pressed();
}

UI::Rect UI::Element::GetReferenceRect()
{
    const Vec2F res = Rendering::Manager::Get().GetResolution();
    const float aspect = res.x / res.y; 
    return {
        Vec2F::Zero(),
        {
            1000.0f * aspect,
            1000.0f
        }
    };
}

UI::Rect UI::Element::ReferenceToViewport(const Rect& InRect)
{
    return { ReferenceToViewport(InRect.Start), ReferenceToViewport(InRect.End) };    
}

Vec2F UI::Element::ReferenceToViewport(const Vec2F& InVec)
{
    // Convert rect from reference space to screen relative
    // First, convert to 0 - 1 space
    // Then, convert to 0 - virtual size
    // TODO: Also consider ref start
    // TODO: Fix stretching
    
    const Vec2F res = Rendering::Manager::Get().GetResolution();
    const Rect ref = GetReferenceRect();
    return {
        (InVec / ref.End) * res,
    };
}

Vec2F UI::Element::ScreenToViewport(const Vec2F& InScreenPos)
{
    // From screen to reference space
    auto& renderer = Rendering::Manager::Get();
    const Vec2F viewportPos = InScreenPos - renderer.GetViewportPosition(); 
    const Vec2F absolute = viewportPos / renderer.GetViewportSize();
    const Rect ref = GetReferenceRect();
    return absolute * ref.End; 
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
        Utility::Math::Lerp(parent.Start.x, parent.End.x, Transform.Anchor.x),
        Utility::Math::Lerp(parent.Start.y, parent.End.y, Transform.Anchor.y)
    };
    
    // Calculate position and size
    Rect absolute;
    absolute.Start = anchor + Transform.Position + Transform.Size * Transform.Pivot;
    absolute.End = anchor + Transform.Position + Transform.Size * (Vec2F(1.0f) - Transform.Pivot);
    
    // And blend to container using alignment
    Rect result;
    result.Start.x = Utility::Math::Lerp(absolute.Start.x, parent.Start.x, Transform.Alignment.Horizontal.x);
    result.Start.y = Utility::Math::Lerp(absolute.Start.y, parent.Start.y, Transform.Alignment.Vertical.x);
    result.End.x = Utility::Math::Lerp(absolute.End.x, parent.End.x, Transform.Alignment.Horizontal.y);
    result.End.y = Utility::Math::Lerp(absolute.End.y, parent.End.y, Transform.Alignment.Vertical.y);
    
    return result; 
}

void UI::Element::DrawRect(const Rect& InRect)
{
    return; 
    const Rect view = ReferenceToViewport(InRect);
    const Vec2F size = view.End - view.Start;
    const Vec2F pos = view.Start;
    DrawRectangleLines(
        static_cast<int>(pos.x + 0.5f),
        static_cast<int>(pos.y + 0.5f),
        static_cast<int>(size.x + 0.5f),    
        static_cast<int>(size.y + 0.5f),
        RED);
}
