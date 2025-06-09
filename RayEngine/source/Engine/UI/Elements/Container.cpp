#include "Container.h"

void UI::Container::Init()
{
    Init(*this);

    // When virtual target is recreated, invalidate hierarchy
    onSetViewportSize.Bind([&](const OnSetViewportSize& InData)
    {
        Invalidate(); 
    });
}

void UI::Container::Update()
{
    Update(*this);
}

void UI::Container::Draw()
{
    RefreshRect(*this, GetReferenceRect());
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
    for (auto& elem : elements)
        elem.second.Get().Draw(*this);
}

bool UI::Container::RefreshRect(Container& InInstance, const Rect& InContainer)
{
    PROFILE();
    if (!Element::RefreshRect(InInstance, InContainer))
        return false;
    
    Rect rect = GetRect();
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;
    
    for (auto& elem : elements)
        elem.second.Get().RefreshRect(*this, rect);

    return true;
    
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