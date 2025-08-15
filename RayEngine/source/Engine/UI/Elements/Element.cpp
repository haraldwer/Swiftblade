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
    if (transform != InTransform)
    {
        transform = InTransform;
        Invalidate();
    }
}

bool UI::Element::IsHovered() const
{
    const Vector2 mp = GetMousePosition();

    auto& view = Rendering::Manager::Get().mainViewport;
    const Rect ref = GetReferenceRect();
    const Vec2F abs = view.ScreenToViewportAbsolute({ mp.x, mp.y });
    const Vec2F screen = abs * ref.end;
    const Vec2F relative = screen;
    return
        relative.x > cachedRect.start.x &&
        relative.x < cachedRect.end.x &&
        relative.y > cachedRect.start.y &&
        relative.y < cachedRect.end.y &&
        Rendering::Manager::Get().IsViewportClickable();
}

bool UI::Element::IsClicked() const
{
    return IsHovered() && Input::Action::Get("LM").Released();
}

bool UI::Element::IsPressed() const
{
    return IsHovered() && Input::Action::Get("LM").Down();
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

UI::Rect UI::Element::CalculateRect(const Rect& InContainer) const
{
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

    Vec2F size = GetDesiredSize();
    
    // Calculate position and size
    Rect absolute;
    absolute.start = anchor + transform.position + size * transform.pivot;
    absolute.end = anchor + transform.position + size * (Vec2F(1.0f) - transform.pivot);
    
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
