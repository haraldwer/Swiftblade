#include "Container.h"

#include "UI/Instance.h"

void UI::Container::Init(Instance& InInstance)
{
    Element::Init(InInstance);
    for (const auto& elem : elements)
        InInstance.Get<Element>(elem).Init(InInstance);
}

void UI::Container::Update(Instance& InInstance)
{
    Element::Update(InInstance);
    for (const auto& elem : elements)
        InInstance.Get<Element>(elem).Update(InInstance);
}

void UI::Container::Draw(Instance& InInstance)
{
    Element::Draw(InInstance);
    for (const auto& elem : elements)
        InInstance.Get<Element>(elem).Draw(InInstance);
}

void UI::Container::RefreshRect(Instance& InInstance, const Rect& InContainer)
{
    cachedRect = CalculateRect(InContainer);
    
    Rect rect = GetRect();
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;
    
    for (const auto& elem : elements)
        InInstance.Get<Element>(elem).RefreshRect(InInstance, rect);
}
