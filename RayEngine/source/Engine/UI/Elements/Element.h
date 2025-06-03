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
        Element() = default;
        Element(const Transform& InTransform) : transform(InTransform) {}
        virtual ~Element() = default;
        
        virtual void Init();
        virtual void Update();
        virtual void Draw();
        virtual void Invalidate();

        Rect GetRect() const { return cachedRect; }
        virtual void RefreshRect(const Rect& InContainer);
        
        Transform GetTransform() const { return transform; }
        void SetTransform(const Transform& InTransform);
        
        bool IsHovered() const;
        bool IsClicked() const;
        
    protected:
        
        static Rect GetReferenceRect();
        static Rect ReferenceToViewport(const Rect& InRect);
        static Vec2F ReferenceToViewport(const Vec2F& InVec);
        static Vec2F ScreenToViewport(const Vec2F& InScreenPos);
        
        Rect CalculateRect(const Rect& InContainer) const;
        static void DrawRect(const Rect& InRect);
        
        WeakPtr<Container> parent = nullptr;
        Transform transform = Transform::Fill();
        Rect cachedRect = {};
    };
}
