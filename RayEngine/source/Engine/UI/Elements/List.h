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
        
        List(const Transform& InTransform, const float InSpacing = 0.0f, const float InSize = 0.0f, const FlowDirection InDirection = FlowDirection::VERTICAL, const bool InReversed = false)
            : Container(InTransform), direction(InDirection), reversed(InReversed), elementSize(InSize), elementSpacing(InSpacing) { }

        bool RefreshRect(Container& InOwner, const Rect& InContainer) override;
        
        void SetSpacing(Container& InOwner, const float InSpacing)
        {
            elementSpacing = InSpacing;
            Invalidate(); 
        }

    private:

        Rect GetChildRect(const Rect& InRect, float InTotal, float InIndex) const;
        
        FlowDirection direction;
        bool reversed;
        float elementSize = 0.0f; 
        float elementSpacing = 0.0f; 
    };
}
