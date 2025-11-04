#pragma once

#include "Basic.h"
#include "Vector/Vector2.hpp"

namespace Utility
{
    namespace Math
    {
        template<class T, class F>
        T Lerp(const T& InA, const T& InB, const F InT)
        {
            return InA + (InB - InA) * Saturate(InT);
        }

        template<class T, class F>
        T LerpDelta(const T& InFrom, const T& InTo, const F InSpeed, const F InDelta)
        {
            F lerp = 1 - pow(1 - InSpeed * 3.0f, InDelta * 300);
            return Lerp(InFrom, InTo, lerp);
        }

        template<class T>
        T SmoothStep(T InT)
        {
            return InT * InT * InT * (InT * (6.0f * InT - 15.0f) + 10.0f);
        }
        
        template<class T, class F>
        T SmoothLerp(const T& InA, const T& InB, const F InT)
        {
            return Lerp(InA, InB, SmoothStep(InT));
        }

        template<class T>
        T Bezier(T t, T p0, T p1, T p2, T p3)
        {
            T u = static_cast<T>(1) - t;
            return
                u * u * u * p0 +
                static_cast<T>(3) * u * u * static_cast<T>(t) * p1 +
                static_cast<T>(3) * u * t * t * p2 + t * t * t * p3;
        }

        template<class T>
        T BezierX(T t, const Vector2<T>& P0, const Vector2<T>& P1, const Vector2<T>& P2, const Vector2<T>& P3)
        {
            return Bezier(t, P0.x, P1.x, P2.x, P3.x);
        }

        template<class T>
        T BezierY(T t, const Vector2<T>& P0, const Vector2<T>& P1, const Vector2<T>& P2, const Vector2<T>& P3)
        {
            return Bezier(t, P0.y, P1.y, P2.y, P3.y);
        }
        
        template<class T>
        T InterpBezier(const T frame, const Vector2<T>& k0, const Vector2<T>& h0, const Vector2<T>& h1, const Vector2<T>& k1)
        {
            T t0 = static_cast<T>(0.0), t1 = static_cast<T>(1.0);
            for (int i = 0; i < 12; ++i) {
                T t = static_cast<T>(0.5) * (t0 + t1);
                T x = BezierX(t, k0, h0, h1, k1);
                if (x < frame)
                    t0 = t;
                else
                    t1 = t;
            }
            T t = static_cast<T>(0.5) * (t0 + t1);
            return BezierY(t, k0, h0, h1, k1);
        }


        
    }
}
