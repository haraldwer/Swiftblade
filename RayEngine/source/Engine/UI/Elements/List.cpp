#include "List.h"

void UI::List::RefreshRect(const Rect& InContainer)
{
    CachedRect = CalculateRect(InContainer);

    Rect rect = GetRect();
    rect.Start.x += Transform.Margin.Horizontal.x;
    rect.End.x -= Transform.Margin.Horizontal.y;
    rect.Start.y += Transform.Margin.Vertical.x;
    rect.End.y -= Transform.Margin.Vertical.y;

    // Each child gets its own rect
    for (int i = 0; i < Elements.size(); i++)
        if (Element* e = Elements[i].Get())
            e->RefreshRect(
                GetChildRect(
                    rect,
                    static_cast<float>(Elements.size()),
                    static_cast<float>(i)));
}

UI::Rect UI::List::GetChildRect(const Rect& InRect, const float total, const float index) const
{
    // Calculate the part for each index
    const float totalSpacing = ElementSpacing * (total - 1.0f);
    const Vec2F diff = ElementSize > 0.001f ?
                           ElementSize : ((InRect.End - InRect.Start) - totalSpacing) / total;
    const float startPart = index;
    const float endPart = index + 1.0f;
        
    // Reverse? 
    const float revStartPart = Reversed ? -startPart : startPart;
    const float revEndPart = Reversed ? -endPart : endPart;
    const float revSpacing = Reversed ? -ElementSpacing : ElementSpacing; 

    // Start of reference 
    const Vec2F referenceStart = Reversed ? InRect.End : InRect.Start;
        
    // Result is start + part
    const Vec2F startResult = referenceStart + diff * revStartPart + Vec2F(revSpacing * startPart);
    const Vec2F endResult = referenceStart + diff * revEndPart + Vec2F(revSpacing * startPart); 
        
    switch (Direction)
    {
    case FlowDirection::VERTICAL:
        return Reversed ? Rect{
               { InRect.Start.x, endResult.y },
               { InRect.End.x, startResult.y }
            } : Rect{
                { InRect.Start.x, startResult.y },
                { InRect.End.x, endResult.y },
            };
    case FlowDirection::HORIZONTAL:
        return Reversed ? Rect{
               { endResult.x, InRect.Start.y },
               { startResult.x, InRect.End.y },
           } : Rect{
               { startResult.x, InRect.Start.y },
               { endResult.x, InRect.End.y },
           };
    }
    return InRect;
}
