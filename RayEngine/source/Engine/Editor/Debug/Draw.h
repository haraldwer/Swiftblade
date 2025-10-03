#pragma once
#include "Rendering/Scene/Instances/DebugShape.h"

namespace Debug
{
    void Line(const Vec3F& InStart, const Vec3F& InEnd, Rendering::Color InColor = Rendering::Color());
    void Sphere(const Vec3F& InPos, float InRadius = 1.0f, Rendering::Color InColor = Rendering::Color(), int InRings = 6, int InSlices = 8);
    void Box(const Vec3F& InPos, const QuatF& InRot = QuatF::Identity(), const Vec3F& InExtent = Vec3F::One(), Rendering::Color InColor = Rendering::Color());
    void Capsule(const Vec3F& InPos, const QuatF& InRot, float InRadius, float InHeight, Rendering::Color InColor = Rendering::Color(), int InSlices = 8);
}
