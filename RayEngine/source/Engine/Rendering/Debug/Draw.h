#pragma once

namespace Rendering
{
    void DebugLine(const Vec3F& InStart, const Vec3F& InEnd, Color InColor = RED);
    void DebugSphere(const Vec3F& InPos, float InRadius = 1.0f, Color InColor = RED, int InRings = 6, int InSlices = 8);
    void DebugBox(const Vec3F& InPos, const QuatF& InRot = QuatF::Identity(), const Vec3F& InExtent = Vec3F::One(), Color InColor = RED);
    void DebugCapsule(const Vec3F& InPos, const QuatF& InRot, float InRadius, float InHeight, Color InColor = RED, int InSlices = 8);
}