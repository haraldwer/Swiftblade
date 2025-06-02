#include "List.h"

void UI::List::RefreshRect(const Rect& InContainer)
{
    CachedRect = CalculateRect(InContainer);

    Rect rect = GetRect();
    rect.start.x += Transform.margins.horizontal.x;
    rect.end.x -= Transform.margins.horizontal.y;
    rect.start.y += Transform.margins.vertical.x;
    rect.end.y -= Transform.margins.vertical.y;

    // Each child gets its own rect
    for (size_t i = 0; i < elements.size(); i++)
        if (Element* e = elements[i].Get())
            e->RefreshRect(
                GetChildRect(
                    rect,
                    static_cast<float>(elements.size()),
                    static_cast<float>(i)));
}

UI::Rect UI::List::GetChildRect(const Rect& InRect, const float total, const float index) const
{
    // Calculate the part for each index
    const float totalSpacing = ElementSpacing * (total - 1.0f);
    const Vec2F diff = ElementSize > 0.001f ?
                           ElementSize : ((InRect.end - InRect.start) - totalSpacing) / total;
    const float startPart = index;
    const float endPart = index + 1.0f;
        
    // Reverse? 
    const float revStartPart = Reversed ? -startPart : startPart;
    const float revEndPart = Reversed ? -endPart : endPart;
    const float revSpacing = Reversed ? -ElementSpacing : ElementSpacing; 

    // Start of reference 
    const Vec2F referenceStart = Reversed ? InRect.end : InRect.start;
        
    // Result is start + part
    const Vec2F startResult = referenceStart + diff * revStartPart + Vec2F(revSpacing * startPart);
    const Vec2F endResult = referenceStart + diff * revEndPart + Vec2F(revSpacing * startPart); 
        
    switch (Direction)
    {
    case FlowDirection::VERTICAL:
        return Reversed ? Rect{
               { InRect.start.x, endResult.y },
               { InRect.end.x, startResult.y }
            } : Rect{
                { InRect.start.x, startResult.y },
                { InRect.end.x, endResult.y },
            };
    case FlowDirection::HORIZONTAL:
        return Reversed ? Rect{
               { endResult.x, InRect.start.y },
               { startResult.x, InRect.end.y },
           } : Rect{
               { startResult.x, InRect.start.y },
               { endResult.x, InRect.end.y },
           };
    }
    return InRect;
}
