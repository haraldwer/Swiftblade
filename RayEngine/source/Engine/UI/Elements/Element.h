#pragma once
#include "../Transform.h"

namespace UI
{
    class Instance;
    class Container;
    typedef uint32 ElementID;

    // A UI element that other elements inherit from
    class Element
    {
        friend class Builder; 

        BASE_TYPE_INFO(Element)
        
    public:
        Element() = default;
        Element(const Transform& InTransform) : transform(InTransform) {}
        virtual ~Element() = default;
        
        virtual void Init(Instance& InInstance);
        virtual void Update(Instance& InInstance);
        virtual void Draw(Instance& InInstance);
        virtual void Invalidate(Instance& InInstance);

        Rect GetRect() const { return cachedRect; }
        virtual void RefreshRect(Instance& InInstance, const Rect& InContainer);
        
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
        
        ElementID parent = -1;
        Transform transform = Transform::Fill();
        Rect cachedRect = {};
    };
}
