#pragma once
#include "../Transform.h"

namespace UI
{
    typedef uint32 ElementID;

    // A UI element that other elements inherit from
    class Element
    {
        friend class Builder; 
        friend class Container;
        
        BASE_TYPE_INFO(Element)
        
    public:
        Element() = default;
        Element(const Transform& InTransform) : transform(InTransform) {}
        virtual ~Element() = default;
        
        virtual void Init(Container& InOwner) {}
        virtual void Update(Container& InOwner) {}
        virtual void Draw(Container& InOwner);

        void Invalidate() { invalidated = true; }
        virtual bool Invalidated() const { return invalidated; }
        
        virtual bool RefreshRect(Container& InOwner, const Rect& InContainingRect);
        Rect GetRect() const { return cachedRect; }
        
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

        ElementID id;
        ElementID parent;
        Transform transform = Transform::Fill();
        Rect cachedRect = {};
        bool invalidated = true;
    };
}
