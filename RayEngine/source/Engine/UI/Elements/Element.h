#pragma once
#include "../Transform.h"

namespace UI
{
    class Container;

    // A UI element that other elements inherit from
    class Element
    {
        friend class Builder; 
    public:
        virtual ~Element() = default;
        Element() = default;
        Element(const Transform& InTransform) : Transform(InTransform) {}
        virtual void Init();
        virtual void Update();
        virtual Rect Draw(const Rect& InContainer);

    protected:
        Rect CalculateRect(const Rect& InContainer) const;
        static void DrawRect(const Rect& InRect);
        ObjectPtr<Container> Parent; 
        Transform Transform = Transform::Fill();
    };
}
