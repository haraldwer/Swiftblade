#pragma once
#include "Container.h"

namespace UI
{
    enum class ListDirection : uint8
    {
        VERTICAL,
        HORIZONTAL
    };
    
    struct ListProperties
    {
        float spacing = 0.0f;
        ListDirection direction = ListDirection::VERTICAL;
        bool reversed = false;
        
        float size = 0.0f; 
        float autoSize = 1.0f;
    };
    
    // A container that contains other UI elements
    class List : public Container
    {
        TYPE_INFO(List, Container)
        
    public:
        List(const Transform& InTransform = Transform::Fill(), const ListProperties& InProperties = {}, const Background& InBg = {}) : Container(InTransform, InBg), properties(InProperties) {}
        void RefreshRect(Container& InOwner, const Rect& InContainer) override;
        Vec2F GetDesiredSize() const override;

    private:

        Rect GetChildRect(const Element& InElem, const Rect& InRect, float InCount, float InIndex, float& InOutTotal) const;
        
        ListProperties properties;
    };
}
