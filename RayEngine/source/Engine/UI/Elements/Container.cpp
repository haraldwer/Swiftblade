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
    CHECK_RETURN(!visible);
    
    auto refRect = GetReferenceRect();
    if (refRect != cachedRefRect)
        Invalidate();
    if (Invalidated())
        RefreshRect(*this, refRect);
    
    Draw(*this);
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
    CHECK_RETURN(!visible);
    PROFILE();
    Element::Draw(InOwner);
    for (auto& child : children)
    {
        CHECK_ASSERT(!elements.contains(child), "Unknown element id");
        elements.at(child).Get().Draw(*this);
    }
}

void UI::Container::RefreshRect(Container& InInstance, const Rect& InContainer)
{
    PROFILE();

    Rect prev = GetRect();
    Element::RefreshRect(InInstance, InContainer);
    cachedRefRect = InContainer;

    Rect rect = GetRect();
    bool changed = prev != rect;
    
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;
    
    for (auto& elem : elements)
        if (changed || elem.second.Get().Invalidated())
            elem.second.Get().RefreshRect(*this, rect);
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
    elements.erase(InID);
    for (int i = static_cast<int>(children.size()) - 1; i >= 0; i--)
        if (children[i] == InID)
            children.erase(children.begin() + i);
}

void UI::Container::ClearChildren()
{
    CHECK_RETURN(children.empty());
    children.clear();
    elements.clear();
    namedElements.clear();
    Invalidate();
}
