#include "Container.h"

void UI::Container::Init()
{
    Element::Init();
    for (auto& elem : elements)
        if (Element* e = elem.Get())
            e->Init();
}

void UI::Container::Update()
{
    Element::Update();
    for (auto& elem : elements)
        if (Element* e = elem.Get())
            e->Update(); 
}

void UI::Container::Draw()
{
    DrawRect(GetRect());
    for (auto& elem : elements)
        if (Element* e = elem.Get())
            e->Draw();
}

void UI::Container::RefreshRect(const Rect& InContainer)
{
    cachedRect = CalculateRect(InContainer);
    
    Rect rect = GetRect();
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;
    
    for (auto& elem : elements)
        if (Element* e = elem.Get())
            e->RefreshRect(rect);
}
