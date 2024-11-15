#include "Container.h"

void UI::Container::Init()
{
    Element::Init();
    for (auto& elem : Elements)
        if (Element* e = elem.Get())
            e->Init();
}

void UI::Container::Update()
{
    Element::Update();
    for (auto& elem : Elements)
        if (Element* e = elem.Get())
            e->Update(); 
}

void UI::Container::Draw()
{
    DrawRect(GetRect());
    for (auto& elem : Elements)
        if (Element* e = elem.Get())
            e->Draw();
}

void UI::Container::RefreshRect(const Rect& InContainer)
{
    CachedRect = CalculateRect(InContainer);
    
    Rect rect = GetRect();
    rect.Start.x += Transform.Margin.Horizontal.x;
    rect.End.x -= Transform.Margin.Horizontal.y;
    rect.Start.y += Transform.Margin.Vertical.x;
    rect.End.y -= Transform.Margin.Vertical.y;
    
    for (auto& elem : Elements)
        if (Element* e = elem.Get())
            e->RefreshRect(rect);
}
