#include "DebugDraw.h"

#include "DebugShapeInstance.h"
#include "Engine/Instance/Instance.h"

void Rendering::DebugLine(const Vec3F& InStart, const Vec3F& InEnd, Color InColor)
{
    Engine::InstanceBase::Get().GetRenderScene().AddDebugLine({
        InStart,
        InEnd,
        InColor
    }); 
}

void Rendering::DebugSphere(const Vec3F& InPos, const float InRadius, const Color InColor, const int InRings, const int InSlices)
{
    Engine::InstanceBase::Get().GetRenderScene().AddDebugShape({
        DebugShapeInstance::Type::SPHERE,
        InPos,
        QuatF::Identity(),
        Vec3F(
            InRadius,
            static_cast<float>(InRings),
            static_cast<float>(InSlices)),
        InColor
    }); 
}

void Rendering::DebugBox(const Vec3F& InPos, const Vec3F& InExtent, const Color InColor)
{
    Engine::InstanceBase::Get().GetRenderScene().AddDebugShape({
        DebugShapeInstance::Type::BOX,
        InPos,
        QuatF::Identity(),
        InExtent,
        InColor
    });
}

void Rendering::DebugCapsule(const Vec3F& InPos, const QuatF& InRot, float InRadius, float InHeight, Color InColor, int InSlices)
{
    Engine::InstanceBase::Get().GetRenderScene().AddDebugShape({
        DebugShapeInstance::Type::CAPSULE,
        InPos,
        InRot,
        Vec3F(
            InRadius,
            InHeight,
            static_cast<float>(InSlices)),
        InColor
    });
}
