#include "SplitContainer.h"

void UI::SplitContainer::RefreshRect(Container &InOwner, const Rect &InContainingRect)
{
    PROFILE();
    
    Rect prev = GetRect();
    Element::RefreshRect(InOwner, InContainingRect);
    cachedRefRect = InContainingRect;

    Rect rect = GetRect();
    bool changed = prev != rect;
    
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;

    Vector<float> ratiosFilled = ratios;
    ratiosFilled.resize(children.size(), 1.0f);
    
    float totalRatio = 0.0f;
    for (auto r : ratios)
        totalRatio += r;
    
    float startRatio = 0.0f;
    for (size_t i = 0; i < children.size(); i++)
    {
        float ratio = 0.0f;
        if (i < ratios.size())
            ratio = ratios[i];
        
        float childRatio = ratio / totalRatio;
        
        Rect childRect = rect;
        float& start = direction == SplitDirection::HORIZONTAL ? childRect.start.x : childRect.start.y; 
        float& end = direction == SplitDirection::HORIZONTAL ? childRect.end.x : childRect.end.y;

        float diff = end - start;
        start = start + diff * startRatio;
        end = start + diff * childRatio;

        if (i != 0)
            start += spacing / 2;
        if (i != children.size() - 1)
            end -= spacing / 2;
        
        auto& elem = Get<Element>(children[i]);
        if (changed || elem.Invalidated())
            elem.RefreshRect(*this, childRect);
        
        startRatio = startRatio + childRatio; 
    }
}
