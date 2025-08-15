#pragma once
#include "../Transform.h"

namespace UI
{
    typedef uint32 ElementID;
    class Container;

    struct Background
    {
        Vec4F color = Vec4F(1, 1, 1, 0);
        float cornerRadius = 0.0f;
    };
    
    // A UI element that other elements inherit from
    class Element
    {
        friend class Builder; 
        friend class Container;
        
        BASE_TYPE_INFO(Element)
        
    public:
        Element() = default;
        Element(const Transform& InTransform, const Background& InBG = {}) : transform(InTransform), background(InBG) {}
        virtual ~Element() = default;
        
        virtual void Init(Container& InOwner) {}
        virtual void Update(Container& InOwner) {}
        virtual void Draw(Container& InOwner);

        void Invalidate() { invalidated = true; }
        virtual bool Invalidated() const { return invalidated; }
        
        virtual void RefreshRect(Container& InOwner, const Rect& InContainingRect);
        Rect GetRect() const { return cachedRect; }
        
        virtual Vec2F GetDesiredSize() const { return transform.size; } 
        
        Transform GetTransform() const { return transform; }
        void SetTransform(const Transform& InTransform);

        Background GetBackground() const { return background; }
        void SetBackground(const Background& InBackground) { background = InBackground; }
        
        virtual bool IsHovered() const;
        bool IsClicked() const;
        bool IsPressed() const;
        
    protected:
        
        static Rect GetReferenceRect();
        static Rect ReferenceToViewport(const Rect& InRect);
        static Vec2F ReferenceToViewport(const Vec2F& InVec);
        
        Rect CalculateRect(const Rect& InContainer) const;
        static void DrawRect(const Rect& InRect);

        ElementID id = -1;
        ElementID parent = -1;
        Transform transform = Transform::Fill();
        Background background = Background();
        Rect cachedRect = {};
        bool invalidated = true;
    };
}
