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
        
        BASE_CLASS_INFO(Element)
        
    public:
        Element() = default;
        Element(const Transform& InTransform, const Background& InBG = {}, bool InScissor = false) : transform(InTransform), background(InBG), scissor(InScissor) {}
        virtual ~Element() = default;
        
        virtual void Init(Container& InOwner) {}
        virtual void Update(Container& InOwner) {}
        virtual void Draw(Container& InOwner);
        virtual bool DebugDraw(Container& InOwner, const String& InIdentifier, int& InC);

        void BeginDraw(Container& InOwner);
        void EndDraw();

        void Invalidate() { invalidated = true; }
        virtual bool Invalidated() const { return invalidated; }
        
        virtual void RefreshRect(Container& InOwner, const Rect& InContainingRect, bool InCacheVisible);
        Rect GetRect() const { return cachedRect; }
        
        virtual Vec2F GetDesiredSize() const;

        Transform GetTransform() const { return transform; }
        void SetTransform(const Transform& InTransform);

        Background GetBackground() const { return background; }
        void SetBackground(const Background& InBackground) { background = InBackground; }

        bool GetVisible() const { return visible; }
        void SetVisible(bool InVisible);

        virtual bool IsHovered() const;
        bool IsClicked() const;
        bool IsPressed() const;
        
    protected:
        
        static Rect GetReferenceRect();
        static Rect ReferenceToViewport(const Rect& InRect);
        static Vec2F ReferenceToViewport(const Vec2F& InVec);
        
        Rect CalculateRect(const Rect& InContainer) const;
        Vec4F GetDrawRect() const;

        void DrawRect(const Vec4F& InRect);
        
        ElementID id = -1;
        ElementID parent = -1;
        Transform transform = Transform::Fill();
        Background background = Background();
        Rect cachedRect = {};
        Vec4F cachedDrawRect = {};
        bool cacheVisible = true;
        bool invalidated = true;
        bool visible = true;
        bool scissor = false;
        bool debugHovered = false;
        bool debugRefreshed = false;
        int debugColor = -1;
    };
}
