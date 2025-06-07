#include "List.h"

#include "UI/Instance.h"

void UI::List::RefreshRect(Instance& InInstance, const Rect& InContainer)
{
    cachedRect = CalculateRect(InContainer);

    Rect rect = GetRect();
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;

    // Each child gets its own rect
    for (size_t i = 0; i < children.size(); i++)
        InInstance.Get<Element>(children[i]).RefreshRect(
            InInstance,
            GetChildRect(
                rect,
                static_cast<float>(children.size()),
                static_cast<float>(i)));
}

UI::Rect UI::List::GetChildRect(const Rect& InRect, const float InTotal, const float InIndex) const
{
    // Calculate the part for each index
    const float totalSpacing = elementSpacing * (InTotal - 1.0f);
    const Vec2F diff = elementSize > 0.001f ?
                           elementSize : ((InRect.end - InRect.start) - totalSpacing) / InTotal;
    const float startPart = InIndex;
    const float endPart = InIndex + 1.0f;
        
    // Reverse? 
    const float revStartPart = reversed ? -startPart : startPart;
    const float revEndPart = reversed ? -endPart : endPart;
    const float revSpacing = reversed ? -elementSpacing : elementSpacing; 

    // Start of reference 
    const Vec2F referenceStart = reversed ? InRect.end : InRect.start;
        
    // Result is start + part
    const Vec2F startResult = referenceStart + diff * revStartPart + Vec2F(revSpacing * startPart);
    const Vec2F endResult = referenceStart + diff * revEndPart + Vec2F(revSpacing * startPart); 
        
    switch (direction)
    {
    case FlowDirection::VERTICAL:
        return reversed ? Rect{
               { InRect.start.x, endResult.y },
               { InRect.end.x, startResult.y }
            } : Rect{
                { InRect.start.x, startResult.y },
                { InRect.end.x, endResult.y },
            };
    case FlowDirection::HORIZONTAL:
        return reversed ? Rect{
               { endResult.x, InRect.start.y },
               { startResult.x, InRect.end.y },
           } : Rect{
               { startResult.x, InRect.start.y },
               { endResult.x, InRect.end.y },
           };
    }
    return InRect;
}
