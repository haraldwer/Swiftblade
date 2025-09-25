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
        
        bool scrollable = false;
        float scrollLerpSpeed = 0.05f;
        float scrollSensitivity = 50.0f;
    };
    
    // A container that contains other UI elements
    class List : public Container
    {
        CLASS_INFO(List, Container)
        
    public:
        List(const Transform& InTransform = Transform::Fill(), const ListProperties& InProperties = {}, const Background& InBg = {}, const bool InScissor = false) : Container(InTransform, InBg, InScissor), properties(InProperties) {}

        void Update(Container &InOwner) override;
        void Draw(Container &InOwner) override;
        void RefreshRect(Container& InOwner, const Rect& InContainer, bool InCacheVisible) override;
        Vec2F GetDesiredSize() const override;
        
    private:

        Rect GetChildRect(const Element& InElem, const Rect& InRect, float InCount, float InIndex, float& InOutTotal) const;
        Vec2F GetChildrenSize() const;
        
        ListProperties properties;
        float targetScrollOffset = 0.0f;
        float scrollOffset = 0.0f;
        float prevScroll = 0.0f;
    };
}
