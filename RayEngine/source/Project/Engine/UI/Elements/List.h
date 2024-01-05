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
        
        List(const UI::Transform& InTransform, float InSpacing = 0.0f, float InSize = 0.0f, FlowDirection InDirection = FlowDirection::VERTICAL, bool InReversed = false)
            : Container(InTransform), Direction(InDirection), Reversed(InReversed), ElementSize(InSize), ElementSpacing(InSpacing) { }

        void RefreshRect(const Rect& InContainer) override;
        
        void SetSpacing(float InSpacing)
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
