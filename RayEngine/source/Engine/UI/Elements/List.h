#pragma once
#include "Container.h"

namespace UI
{
    // A container that contains other UI elements
    class List : public Container
    {
        friend class Builder; 
    public:

        enum class FlowDirection : uint8
        {
            VERTICAL,
            HORIZONTAL
        };
        
        List(const UI::Transform& InTransform, const float InSpacing = 0.0f, const float InSize = 0.0f, const FlowDirection InDirection = FlowDirection::VERTICAL, const bool InReversed = false)
            : Container(InTransform), Direction(InDirection), Reversed(InReversed), ElementSize(InSize), ElementSpacing(InSpacing) { }

        void RefreshRect(const Rect& InContainer) override;
        
        void SetSpacing(const float InSpacing)
        {
            ElementSpacing = InSpacing;
            Invalidate(); 
        }

    private:

        Rect GetChildRect(const Rect& InRect, const float total, const float index) const;;
        
        FlowDirection Direction;
        bool Reversed;
        float ElementSize = 0.0f; 
        float ElementSpacing = 0.0f; 
    };
}
