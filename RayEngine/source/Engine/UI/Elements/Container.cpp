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