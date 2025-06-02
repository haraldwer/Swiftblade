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
    CachedRect = CalculateRect(InContainer);
    
    Rect rect = GetRect();
    rect.start.x += Transform.margins.horizontal.x;
    rect.end.x -= Transform.margins.horizontal.y;
    rect.start.y += Transform.margins.vertical.x;
    rect.end.y -= Transform.margins.vertical.y;
    
    for (auto& elem : elements)
        if (Element* e = elem.Get())
            e->RefreshRect(rect);
}
