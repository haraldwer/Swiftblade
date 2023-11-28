#pragma once
#include "Element.h"

namespace UI
{
    // A container that contains other UI elements
    class Container : public Element
    {
        friend class Builder; 
    public:
        Container() = default; 
        Container(const UI::Transform& InTransform) : Element(InTransform) {}
        void Init() override;
        void Update() override;
        Rect Draw(const Rect& InContainer) override;
        
    protected: 
        Vector<ObjectPtr<Element>> Elements;
    };
}
