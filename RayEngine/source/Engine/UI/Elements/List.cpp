#include "List.h"

void UI::List::RefreshRect(Container& InOwner, const Rect& InContainingRect)
{
    PROFILE();

    Container::RefreshRect(InOwner, InContainingRect);
    cachedRefRect = InContainingRect;
    
    Rect rect = GetRect();
    rect.start.x += transform.margins.horizontal.x;
    rect.end.x -= transform.margins.horizontal.y;
    rect.start.y += transform.margins.vertical.x;
    rect.end.y -= transform.margins.vertical.y;

    // Each child gets its own rect
    float totalOffset = 0.0f;
    for (size_t i = 0; i < children.size(); i++)
    {
        auto& elem = Get<Element>(children[i]);
        auto childRect = GetChildRect(
            elem,
            rect,
            static_cast<float>(children.size()),
            static_cast<float>(i),
            totalOffset);
        elem.RefreshRect(
            *this,
            childRect);
    }
}

Vec2F UI::List::GetDesiredSize() const
{
    if (elementSize < 0.0001f)
    {
        float totalOffset = 0.0f;
        float maxChildSize = 0.0f;
        Rect rect = GetRect();
        for (size_t i = 0; i < children.size(); i++)
        {
            auto& elem = Get<Element>(children[i]);
            GetChildRect(
                elem,
                rect,
                static_cast<float>(children.size()),
                static_cast<float>(i),
                totalOffset);
            Vec2F s = elem.GetDesiredSize();
            maxChildSize = Utility::Math::Max(maxChildSize,
                direction == FlowDirection::HORIZONTAL ?
                    s.y : s.x);
        }

        return direction == FlowDirection::HORIZONTAL ?
            Vec2F(totalOffset, maxChildSize) :
            Vec2F(maxChildSize, totalOffset);
    }
    return Container::GetDesiredSize();
}

UI::Rect UI::List::GetChildRect(const Element& InElem, const Rect& InRect, const float InCount, const float InIndex, float& InOutTotal) const
{
    Vec2F start = reversed ? InRect.end : InRect.start;

    if (InIndex > 0.0)
        InOutTotal += elementSpacing;
    
    float rev = reversed ? -1.0f : 1.0f;
    // Rect is based on desired size
    Vec2F desiredSize = InElem.GetDesiredSize();
    Vec2F dirStart = direction == FlowDirection::HORIZONTAL ?
        Vec2F(InOutTotal, 0) : Vec2F(0, InOutTotal);
    Vec2F autoSizeStart = start + dirStart * rev;
    InOutTotal += direction == FlowDirection::HORIZONTAL ? desiredSize.x : desiredSize.y;
    Vec2F dirEnd = direction == FlowDirection::HORIZONTAL ?
                Vec2F(InOutTotal, 0) : Vec2F(0, InOutTotal);
    Vec2F autoSizeEnd = start + dirEnd * rev;

    
    // Calculate the part for each index
    const float totalSpacing = elementSpacing * (InCount - 1.0f);
    const Vec2F diff = elementSize > 0.001f ? elementSize : ((InRect.end - InRect.start) - totalSpacing) / InCount;
    const float startPart = InIndex;
    const float endPart = InIndex + 1.0f;
        
    // Reverse? 
    const float revStartPart = reversed ? -startPart : startPart;
    const float revEndPart = reversed ? -endPart : endPart;
    const float revSpacing = reversed ? -elementSpacing : elementSpacing; 

    // Start of reference 
    const Vec2F referenceStart = reversed ? InRect.end : InRect.start;
        
    // Result is start + part
    Vec2F startResult = referenceStart + diff * revStartPart + Vec2F(revSpacing * startPart);
    Vec2F endResult = referenceStart + diff * revEndPart + Vec2F(revSpacing * startPart);


    startResult = Utility::Math::Lerp(startResult, autoSizeStart, elementAutoSize);
    endResult = Utility::Math::Lerp(endResult, autoSizeEnd, elementAutoSize);
        
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
