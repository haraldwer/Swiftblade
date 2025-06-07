#pragma once
#include "Element.h"

namespace UI
{
    // A container that contains other UI elements
    class Container : public Element
    {
        friend class Builder;
        
        TYPE_INFO(Container, Element)
        
    public:
        Container() = default;
        Container(const Transform& InTransform) : Element(InTransform) {}
        void Init(Instance& InInstance) override;
        void Update(Instance& InInstance) override;
        void Draw(Instance& InInstance) override;

    protected:
        void RefreshRect(Instance& InInstance, const Rect& InContainer) override;
        Vector<ElementID> children = {};
    };
}
