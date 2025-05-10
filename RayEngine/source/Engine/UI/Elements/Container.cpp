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
    rect.Start.x += Transform.Margins.Horizontal.x;
    rect.End.x -= Transform.Margins.Horizontal.y;
    rect.Start.y += Transform.Margins.Vertical.x;
    rect.End.y -= Transform.Margins.Vertical.y;
    
    for (auto& elem : Elements)
        if (Element* e = elem.Get())
            e->RefreshRect(rect);
}
