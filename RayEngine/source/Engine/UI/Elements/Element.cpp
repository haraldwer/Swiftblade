#include "Element.h"

#include "Container.h"
#include "Input/Manager.h"
#include "Rendering/Manager.h"
#include "raylib.h"
#include "Math/Random.hpp"

void UI::Element::BeginDraw(Container &InOwner)
{
    CHECK_RETURN(!cacheVisible);
    
    if (Rendering::Manager::Get().GetConfig().DrawElementRects.Get())
        DrawRect(cachedDrawRect);

    if (scissor)
        BeginScissorMode(
            cachedDrawRect.x,
            cachedDrawRect.y,
            cachedDrawRect.z,
            cachedDrawRect.w);
}

void UI::Element::EndDraw()
{
    CHECK_RETURN(!cacheVisible);
    
    if (scissor)
        EndScissorMode();
}

void UI::Element::Draw(Container& InOwner)
{
    CHECK_RETURN(!cacheVisible);
    
    if (background.color.a > 0.001f)
    {
        const auto r = cachedDrawRect;
        DrawRectangleRounded(
            { r.x, r.y, r.z, r.w},
            background.cornerRadius,
            16,
            {
                static_cast<uint8>(background.color.r * 255),
                static_cast<uint8>(background.color.g * 255),
                static_cast<uint8>(background.color.b * 255),
                static_cast<uint8>(background.color.a * 255)
            });
    }
}

bool UI::Element::DebugDraw(Container &InOwner, const String &InIdentifier, int& InC)
{
#ifdef IMGUI_ENABLE
    
    InC++;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth;
    if (!IsA(Type::Get<Container>()))
        flags |= ImGuiTreeNodeFlags_Leaf;

    String info;
    
    if (!visible)
        info = "hidden";
    else if (!cacheVisible)
        info = "cacheHidden";
    else if (invalidated)
        info = "invalidated";
    if (!info.empty())
        info = " (" + info + ")";
    if (debugRefreshed)
    {
        debugRefreshed = false;
        info += " *";
    }

    bool treeNode = ImGui::TreeNodeEx(("[" + GetObjName() + "]: " + InIdentifier + info + " ##" + Utility::ToStr(InC)).c_str(), flags); 
    debugHovered = ImGui::IsItemHovered();
    return treeNode;
    
#endif
    
    return false;
}

void UI::Element::RefreshRect(Container& InOwner, const Rect& InContainingRect, bool InCacheVisible)
{
    debugRefreshed = true;
    invalidated = false;
    cacheVisible = visible && InCacheVisible;
    
    CHECK_RETURN(!cacheVisible);
    cachedRect = CalculateRect(InContainingRect);
    cachedDrawRect = GetDrawRect();
}

Vec2F UI::Element::GetDesiredSize() const
{
    return transform.size;
}

void UI::Element::SetTransform(const Transform& InTransform)
{
    if (transform != InTransform)
    {
        transform = InTransform;
        Invalidate();
    }
}

void UI::Element::SetVisible(const bool InVisible)
{
    if (visible != InVisible)
    {
        visible = InVisible;
        invalidated = true;
    }
}

bool UI::Element::IsHovered() const
{
    CHECK_RETURN(!cacheVisible, false);
    
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
    const Rect ref = GetReferenceRect();
    return (InVec / ref.end) * resi.To<float>();
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
    absolute.start = anchor + transform.position - size * transform.pivot;
    absolute.end = anchor + transform.position + size * (Vec2F(1.0f) - transform.pivot);
    
    // And blend to container using alignment
    Rect result;
    result.start.x = Utility::Math::Lerp(absolute.start.x, parentRect.start.x, transform.alignment.horizontal.x);
    result.start.y = Utility::Math::Lerp(absolute.start.y, parentRect.start.y, transform.alignment.vertical.x);
    result.end.x = Utility::Math::Lerp(absolute.end.x, parentRect.end.x, transform.alignment.horizontal.y);
    result.end.y = Utility::Math::Lerp(absolute.end.y, parentRect.end.y, transform.alignment.vertical.y);

    constexpr float precision = 10.0f;
    result.start.x = round(result.start.x * precision) / precision;
    result.start.y = round(result.start.y * precision) / precision;
    result.end.x = round(result.end.x * precision) / precision;
    result.end.y = round(result.end.y * precision) / precision;
    
    return result; 
}

Vec4F UI::Element::GetDrawRect() const
{
    const Rect view = ReferenceToViewport(cachedRect);
    const Vec2F size = view.end - view.start;
    const Vec2F pos = view.start;
    return { pos, size };
}

void UI::Element::DrawRect(const Vec4F& InRect)
{
    constexpr Array<Color, 25> colors
    {
        ::LIGHTGRAY,
        ::GRAY,
        ::DARKGRAY,
        ::YELLOW,
        ::GOLD,
        ::ORANGE,
        ::PINK,
        ::RED,
        ::MAROON,
        ::GREEN,
        ::LIME,
        ::DARKGREEN,
        ::SKYBLUE,
        ::BLUE,
        ::DARKBLUE,
        ::PURPLE,
        ::VIOLET,
        ::DARKPURPLE,
        ::BEIGE,
        ::BROWN,
        ::DARKBROWN,
        ::WHITE,
        ::BLACK,
        ::BLANK,
        ::MAGENTA,
    };


    if (debugColor < 0 || debugColor >= static_cast<int>(colors.size()))
    {
        Random rnd = Utility::Hash(InRect);
        const float f = rnd.Factor<float>();
        const int i = static_cast<int>(std::floor(f * 25));
        debugColor = i;
    }
    
    const Color c = colors[debugColor];
    float thickness = debugHovered ? 8.0f : 1.2f;
    DrawRectangleLinesEx({ InRect.x, InRect.y, InRect.z, InRect.w}, thickness, c);
}

