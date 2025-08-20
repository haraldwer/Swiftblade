#pragma once
#include "Container.h"

namespace UI
{
    // A container that contains other UI elements
    class List : public Container
    {
        TYPE_INFO(List, Container)
        
    public:

        enum class FlowDirection : uint8
        {
            VERTICAL,
            HORIZONTAL
        };
        
        List(const Transform& InTransform = Transform::Fill(), const float InSpacing = 0.0f, const float InElementSize = 0.0f, const FlowDirection InDirection = FlowDirection::VERTICAL, const bool InReversed = false, const float InAutoSize = 1.0f)
            : Container(InTransform), direction(InDirection), reversed(InReversed), elementSize(InElementSize), elementSpacing(InSpacing), elementAutoSize(InAutoSize) { }

        void RefreshRect(Container& InOwner, const Rect& InContainer) override;
        Vec2F GetDesiredSize() const override;
        
        void SetSpacing(Container& InOwner, const float InSpacing)
        {
            elementSpacing = InSpacing;
            Invalidate(); 
        }

    private:

        Rect GetChildRect(const Element& InElem, const Rect& InRect, float InCount, float InIndex, float& InOutTotal) const;
        
        FlowDirection direction;
        bool reversed;
        float elementSize = 0.0f; 
        float elementSpacing = 0.0f;
        float elementAutoSize = 0.0f;
    };
}
