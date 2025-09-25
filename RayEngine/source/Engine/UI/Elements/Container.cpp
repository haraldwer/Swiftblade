#include "Container.h"

void UI::Container::Init()
{
    Init(*this);
}

void UI::Container::Update()
{
    Update(*this);
}

void UI::Container::Draw()
{
    auto refRect = GetReferenceRect();
    if (refRect != cachedRefRect)
        Invalidate();
    if (Invalidated())
        RefreshRect(*this, refRect, true);

    CHECK_RETURN(!cacheVisible);
    
    BeginDraw(*this);
    Draw(*this);
    EndDraw();
}

void UI::Container::DebugDraw(int& InC)
{
    if (DebugDraw(*this, "Root", InC))
        ImGui::TreePop();
}

void UI::Container::Init(Container& InOwner)
{
    PROFILE();
    Element::Init(InOwner);
    for (auto& elem : elements)
        elem.second.Get().Init(*this);
}

void UI::Container::Update(Container& InOwner)
{
    PROFILE();
    Element::Update(InOwner);
    for (auto& elem : elements)
        elem.second.Get().Update(*this);
}

void UI::Container::Draw(Container& InOwner)
{
    CHECK_RETURN(!cacheVisible);
    
    PROFILE();
    Element::Draw(InOwner);
    for (auto& child : children)
    {
        CHECK_ASSERT(!elements.contains(child), "Unknown element id");
        auto& elem = elements.at(child).Get();
        CHECK_CONTINUE(!elem.cacheVisible);
        elem.BeginDraw(*this);
        elem.Draw(*this);
        elem.EndDraw();
    }
}

bool UI::Container::DebugDraw(Container &InOwner, const String &InIdentifier, int& InC)
{
    if (Element::DebugDraw(InOwner, InIdentifier, InC))
    {
        for (auto& child : children)
        {
            CHECK_ASSERT(!elements.contains(child), "Unknown element id");
            auto& elem = elements.at(child).Get();
            String identifier = "";
            if (auto find = elementNames.find(child); find != elementNames.end())
                identifier = find->second;
            if (elem.DebugDraw(InOwner, identifier, InC))
                ImGui::TreePop();
        }
        return true;
    }
    return false;
}

void UI::Container::RefreshRect(Container& InInstance, const Rect& InContainer, bool InCacheVisible)
{
    PROFILE();

    bool prevVisible = cacheVisible;
    Rect prev = GetRect();
    Element::RefreshRect(InInstance, InContainer, InCacheVisible);
    cachedRefRect = InContainer;

    Rect rect = GetRect();
    bool changed = prev != rect || prevVisible != cacheVisible;
    
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;
    
    for (auto& elem : elements)
        if (changed || elem.second.Get().Invalidated())
            elem.second.Get().RefreshRect(*this, rect, cacheVisible);
}

Vec2F UI::Container::GetDesiredSize() const
{
    Vec2F margin = {
        transform.margins.horizontal.x + transform.margins.horizontal.y, 
        transform.margins.vertical.x + transform.margins.vertical.y
    }; 
    Vec2F max = transform.size + margin;
    for (auto& el : elements)
    {
        Vec2F s = el.second.Get().GetDesiredSize() + margin;
        max = {
            Utility::Math::Max(max.x, s.x),
            Utility::Math::Max(max.y, s.y)
        };
    }
    Vec2F padding = {
        transform.padding.horizontal.x + transform.padding.horizontal.y, 
        transform.padding.vertical.x + transform.padding.vertical.y
    };
    return max + padding;
}

bool UI::Container::IsHovered() const
{
    for (auto& e : elements)
        if (e.second.Get<>().IsHovered())
            return true;
    return false;
}

bool UI::Container::Invalidated() const
{
    if (Element::Invalidated())
        return true;
    for (auto& e : elements)
        if (e.second.Get<>().Invalidated())
            return true;
    return false;
}

UI::ElementID UI::Container::GetID(const String& InName) const
{
    if (InName == "Root")
        return 0;
    auto find = namedElements.find(InName);
    CHECK_RETURN(find == namedElements.end(), -1);
    return find->second;
}

void UI::Container::Remove(const ElementID InID)
{
    CHECK_ASSERT(!elements.contains(InID), "Does not contain element")
    if (elementNames.contains(id))
    {
        namedElements.erase(elementNames.at(id));
        elementNames.erase(id);
    }
    elements.erase(id);
    for (int i = static_cast<int>(children.size()) - 1; i >= 0; i--)
        if (children[i] == InID)
            children.erase(children.begin() + i);
}

void UI::Container::RemoveChild(const int InIndex)
{
    CHECK_ASSERT(InIndex < 0 || InIndex >= children.size(), "Invalid index");
    Remove(children.at(InIndex));
}

void UI::Container::ClearChildren()
{
    CHECK_RETURN(children.empty());
    children.clear();
    elements.clear();
    namedElements.clear();
    elementNames.clear();
    Invalidate();
}
