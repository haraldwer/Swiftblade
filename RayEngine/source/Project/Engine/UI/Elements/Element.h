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
        virtual void Draw();
        virtual void Invalidate();

        Rect GetRect() const { return CachedRect; }
        virtual void RefreshRect(const Rect& InContainer);
        
        bool IsHovered() const;
        bool IsClicked() const;
        
    protected:
        
        static Rect GetReferenceRect();
        static Rect ToViewport(const Rect& InRect);
        static Vec2F ToViewport(const Vec2F& InVec);
        static Vec2F FromScreen(const Vec2F& InScreenPos);
        
        Rect CalculateRect(const Rect& InContainer) const;
        static void DrawRect(const Rect& InRect);
        
        ObjectPtr<Container> Parent;
        Transform Transform = Transform::Fill();
        Rect CachedRect;
    };
}
