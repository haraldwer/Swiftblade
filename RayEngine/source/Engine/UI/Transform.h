#pragma once

namespace UI
{
    struct Margin
    {
        Margin() = default;
        Margin(const Vec2F& InH, const Vec2F& InV) : Horizontal(InH), Vertical(InV) {}
        Margin(float InV) : Horizontal(InV), Vertical(InV) {}
        
        Vec2F Horizontal = {};
        Vec2F Vertical = {};

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
    };
    
    struct Rect
    {
        Vec2F Start = {};
        Vec2F End = {}; 
    };
    
    struct Transform  
    {
        Vec2F Position = {};
        Vec2F Size = {}; 
        Margin Alignment = {};
        Margin Padding = {};
        Margin Margins = {};
        Vec2F Anchor = {};
        Vec2F Pivot = {};
        float Rotation = 0.0;

        static Transform Fill(const struct Margin& InPadding = Margin::Zero(), const struct Margin& InMargin = Margin::Zero())
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

        static Transform SimpleAnchor(const Vec2F& InAnchor)
        {
            return {
                Vec2F::Zero(),
                Vec2F::Zero(),
                Margin::One(),
                Margin::Zero(),
                Margin::Zero(),
                InAnchor,
                Vec2F::Zero(),
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
    };
}