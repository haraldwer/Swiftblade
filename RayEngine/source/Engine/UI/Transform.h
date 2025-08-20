#pragma once

namespace UI
{
    struct Margin
    {
        Margin() = default;
        Margin(const Vec2F& InH, const Vec2F& InV) : horizontal(InH), vertical(InV) {}
        Margin(const float InV) : horizontal(InV), vertical(InV) {}
        
        Vec2F horizontal = {};
        Vec2F vertical = {};

        static Margin One()
        {
            return {
                Vec2F::One(),
                Vec2F::One()
            };
        }

        static Margin Zero()
        {
            return {
                Vec2F::Zero(),
                Vec2F::Zero()
            };
        }

        bool operator==(const Margin & InMargin) const
        {
            return
                horizontal == InMargin.horizontal &&
                vertical == InMargin.vertical;
        }
    };
    
    struct Rect
    {
        bool operator == (const Rect& InOther) const
        {
            return
                start == InOther.start &&
                end == InOther.end;
        }

        Vec2F start = {};
        Vec2F end = {}; 
    };
    
    struct Transform  
    {
        // Position - The actual position information
        // Size - The actual size information
        // Alignment - How the position and size should be used relative to parent
        // Padding - Padding around the current element
        // Anchor - Reference point on InContainer
        // Pivot - Size point relative to position
        // Margin - Margin inside the current element, used by container for child elements
        
        Vec2F position = {};
        Vec2F size = {}; 
        Margin alignment = {};
        Margin padding = {};
        Margin margins = {};
        Vec2F anchor = {};
        Vec2F pivot = {};
        float rotation = 0.0;
        
        static Transform Fill(const Margin& InPadding = Margin::Zero(), const Margin& InMargin = Margin::Zero())
        {
            return {
                Vec2F::Zero(),
                Vec2F::Zero(),
                Margin::One(),
                InPadding,
                InMargin,
                Vec2F::Zero(),
                Vec2F::Zero(),
                0.0
            };
        }

        static Transform Centered()
        {
            return {
                Vec2F::Zero(),
                Vec2F::Zero(),
                Margin::Zero(),
                Margin::Zero(),
                Margin::Zero(),
                0.5,
                0.5,
                0.0
            };
        }

        static Transform SimpleAnchor(const Vec2F& InAnchor, const Vec2F& InPivot = Vec2F::Zero())
        {
            return {
                Vec2F::Zero(),
                Vec2F::Zero(),
                Margin::One(),
                Margin::Zero(),
                Margin::Zero(),
                InAnchor,
                InPivot,
                0.0
            };
        }

        static Transform FromRect(const Vec2F& InSize, const Vec2F& InPosition = Vec2F::Zero(), const Vec2F& InAnchor = Vec2F::Zero(), const Vec2F& InPivot = Vec2F::Zero())
        {
            return {
                InPosition,
                InSize,
                Margin::Zero(),
                Margin::Zero(),
                Margin::Zero(),
                InAnchor,
                InPivot,
                0.0
            };
        }

        bool operator==(const Transform & InTransform) const
        {
            return
                position == InTransform.position &&
                size == InTransform.size &&
                alignment == InTransform.alignment &&
                padding == InTransform.padding &&
                margins == InTransform.margins &&
                pivot == InTransform.pivot &&
                anchor == InTransform.anchor &&
                rotation == InTransform.rotation;
        };
    };
}