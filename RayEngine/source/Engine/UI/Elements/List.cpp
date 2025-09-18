#include "List.h"

void UI::List::RefreshRect(Container& InOwner, const Rect& InContainingRect, bool InCacheVisible)
{
    PROFILE();

    bool prevVisible = cacheVisible;
    Rect prev = GetRect();
    Element::RefreshRect(InOwner, InContainingRect, InCacheVisible);
    cachedRefRect = InContainingRect;
    
    Rect rect = GetRect();
    bool changed = prev != rect || prevVisible != cacheVisible;

    if (!changed)
        for (auto& c : children)
            if (Get<Element>(c).Invalidated())
                changed = true;
    
    CHECK_RETURN(!changed);
    
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
            childRect,
            cacheVisible);
    }
}

Vec2F UI::List::GetDesiredSize() const
{
    if (properties.size < 0.0001f)
    {
        Vec2F margin = {
            transform.margins.horizontal.x + transform.margins.horizontal.y, 
            transform.margins.vertical.x + transform.margins.vertical.y
        }; 
        
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
                properties.direction == ListDirection::HORIZONTAL ?
                    s.y : s.x);
        }

        return (properties.direction == ListDirection::HORIZONTAL ?
            Vec2F(totalOffset, maxChildSize) :
            Vec2F(maxChildSize, totalOffset)) + margin;
    }
    return Container::GetDesiredSize();
}

UI::Rect UI::List::GetChildRect(const Element& InElem, const Rect& InRect, const float InCount, const float InIndex, float& InOutTotal) const
{
    Vec2F start = properties.reversed ? InRect.end : InRect.start;

    if (InIndex > 0.0)
        InOutTotal += properties.spacing;
    
    float rev = properties.reversed ? -1.0f : 1.0f;
    // Rect is based on desired size
    Vec2F elemSize = InElem.GetDesiredSize();
    Vec2F desiredSize = {
        Utility::Math::Max(elemSize.x, properties.size),
        Utility::Math::Max(elemSize.y, properties.size),
    };
    
    Vec2F dirStart = properties.direction == ListDirection::HORIZONTAL ?
        Vec2F(InOutTotal, 0) : Vec2F(0, InOutTotal);
    Vec2F autoSizeStart = start + dirStart * rev;
    InOutTotal += properties.direction == ListDirection::HORIZONTAL ? desiredSize.x : desiredSize.y;
    Vec2F dirEnd = properties.direction == ListDirection::HORIZONTAL ?
                Vec2F(InOutTotal, 0) : Vec2F(0, InOutTotal);
    Vec2F autoSizeEnd = start + dirEnd * rev;
    
    // Calculate the part for each index
    const float totalSpacing = properties.spacing * (InCount - 1.0f);
    const Vec2F diff = properties.size > 0.001f ? properties.size : ((InRect.end - InRect.start) - totalSpacing) / InCount;
    const float startPart = InIndex;
    const float endPart = InIndex + 1.0f;
        
    // Reverse? 
    const float revStartPart = properties.reversed ? -startPart : startPart;
    const float revEndPart = properties.reversed ? -endPart : endPart;
    const float revSpacing = properties.reversed ? -properties.spacing : properties.spacing; 

    // Start of reference 
    const Vec2F referenceStart = properties.reversed ? InRect.end : InRect.start;
        
    // Result is start + part
    Vec2F startResult = referenceStart + diff * revStartPart + Vec2F(revSpacing * startPart);
    Vec2F endResult = referenceStart + diff * revEndPart + Vec2F(revSpacing * startPart);


    startResult = Utility::Math::Lerp(startResult, autoSizeStart, properties.autoSize);
    endResult = Utility::Math::Lerp(endResult, autoSizeEnd, properties.autoSize);
        
    switch (properties.direction)
    {
    case ListDirection::VERTICAL:
        return properties.reversed ? Rect{
               { InRect.start.x, endResult.y },
               { InRect.end.x, startResult.y }
            } : Rect{
                { InRect.start.x, startResult.y },
                { InRect.end.x, endResult.y },
            };
    case ListDirection::HORIZONTAL:
        return properties.reversed ? Rect{
               { endResult.x, InRect.start.y },
               { startResult.x, InRect.end.y },
           } : Rect{
               { startResult.x, InRect.start.y },
               { endResult.x, InRect.end.y },
           };
    }
    return InRect;
}
