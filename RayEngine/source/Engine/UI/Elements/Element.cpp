#include "Element.h"

#include "Input/Manager.h"
#include "Rendering/Manager.h"
#include "raylib.h"

void UI::Element::Draw(Container& InOwner)
{
    DrawRect(cachedRect); 
}

void UI::Element::RefreshRect(Container& InOwner, const Rect& InContainingRect)
{
    cachedRect = CalculateRect(InContainingRect);
    invalidated = false;
}

void UI::Element::SetTransform(const Transform& InTransform)
{
    transform = InTransform;
}

bool UI::Element::IsHovered() const
{
    const Vector2 mp = GetMousePosition();
    const Vec2F relative = ScreenToViewport({ mp.x, mp.y });
    return
        relative.x > cachedRect.start.x &&
        relative.x < cachedRect.end.x &&
        relative.y > cachedRect.start.y &&
        relative.y < cachedRect.end.y;
}

bool UI::Element::IsClicked() const
{
    return IsHovered() && Input::Action::Get("LM").Pressed() && Rendering::Manager::Get().IsViewportClickable();
}

UI::Rect UI::Element::GetReferenceRect()
{
    const Vec2I res = Rendering::Manager::Get().mainViewport.GetResolution();
    const float aspect = static_cast<float>(res.x) / static_cast<float>(res.y); 
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
    return { ReferenceToViewport(InRect.start), ReferenceToViewport(InRect.end) };    
}

Vec2F UI::Element::ReferenceToViewport(const Vec2F& InVec)
{
    // Convert rect from reference space to screen relative
    // First, convert to 0 - 1 space
    // Then, convert to 0 - virtual size
    // TODO: Also consider ref start
    // TODO: Fix stretching
    
    const Vec2I resi = Rendering::Manager::Get().mainViewport.GetResolution();
    const Vec2F res = { static_cast<float>(resi.x), static_cast<float>(resi.y) };
    const Rect ref = GetReferenceRect();
    return {
        (InVec / ref.end) * res,
    };
}

Vec2F UI::Element::ScreenToViewport(const Vec2F& InScreenPos)
{
    // From screen to reference space
    const auto& man = Rendering::Manager::Get();
    const Vec2F viewportPos = InScreenPos - man.mainViewport.GetPosition();
    const Vec2I si = man.mainViewport.GetSize();
    const Vec2F s = { static_cast<float>(si.x), static_cast<float>(si.y) };
    const Vec2F absolute = viewportPos / s;
    const Rect ref = GetReferenceRect();
    return absolute * ref.end; 
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
    Rect parentRect = InContainer;
    parentRect.start.x += transform.padding.horizontal.x;
    parentRect.end.x -= transform.padding.horizontal.y;
    parentRect.start.y += transform.padding.vertical.x;
    parentRect.end.y -= transform.padding.vertical.y;

    // Calculate anchor point
    const Vec2F anchor = {
        Utility::Math::Lerp(parentRect.start.x, parentRect.end.x, transform.anchor.x),
        Utility::Math::Lerp(parentRect.start.y, parentRect.end.y, transform.anchor.y)
    };
    
    // Calculate position and size
    Rect absolute;
    absolute.start = anchor + transform.position + transform.size * transform.pivot;
    absolute.end = anchor + transform.position + transform.size * (Vec2F(1.0f) - transform.pivot);
    
    // And blend to container using alignment
    Rect result;
    result.start.x = Utility::Math::Lerp(absolute.start.x, parentRect.start.x, transform.alignment.horizontal.x);
    result.start.y = Utility::Math::Lerp(absolute.start.y, parentRect.start.y, transform.alignment.vertical.x);
    result.end.x = Utility::Math::Lerp(absolute.end.x, parentRect.end.x, transform.alignment.horizontal.y);
    result.end.y = Utility::Math::Lerp(absolute.end.y, parentRect.end.y, transform.alignment.vertical.y);
    
    return result; 
}

void UI::Element::DrawRect(const Rect& InRect)
{
    const Rect view = ReferenceToViewport(InRect);
    const Vec2F size = view.end - view.start;
    const Vec2F pos = view.start;
    DrawRectangleLines(
        static_cast<int>(pos.x + 0.5f),
        static_cast<int>(pos.y + 0.5f),
        static_cast<int>(size.x + 0.5f),    
        static_cast<int>(size.y + 0.5f),
        ::RED);
}
