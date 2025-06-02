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
    };
    
    struct Rect
    {
        Vec2F start = {};
        Vec2F end = {}; 
    };
    
    struct Transform  
    {
        Vec2F position = {};
        Vec2F size = {}; 
        Margin alignment = {};
        Margin padding = {};
        Margin margins = {};
        Vec2F anchor = {};
        Vec2F pivot = {};
        float rotation = 0.0;

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