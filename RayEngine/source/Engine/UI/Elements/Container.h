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
        ~Container() override = default; 
        Container(const UI::Transform& InTransform) : Element(InTransform) {}
        void Init() override;
        void Update() override;
        void Draw() override;

    protected:
        void RefreshRect(const Rect& InContainer) override;
        Vector<ObjectPtr<Element>> Elements;
    };
}
