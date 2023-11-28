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

UI::Rect UI::Container::Draw(const Rect& InContainer)
{
    Element::Draw(InContainer); 
    
    Rect rect = CalculateRect(InContainer);
    rect.Start.x -= Transform.Margin.Horizontal.x;
    rect.End.x += Transform.Margin.Horizontal.y;
    rect.Start.y -= Transform.Margin.Vertical.x;
    rect.End.y += Transform.Margin.Vertical.y;
    
    for (auto& elem : Elements)
        if (Element* e = elem.Get())
            e->Draw(rect);

    return rect;
}
