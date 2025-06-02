#include "Draw.h"

#include "Engine/Instance/Instance.h"
#include "Rendering/Scene/Instances/DebugShape.h"

void Engine::DebugLine(const Vec3F& InStart, const Vec3F& InEnd, const Rendering::Color InColor)
{
    Engine::Instance::Get().GetRenderScene().AddDebugLine({
        InStart,
        InEnd,
        InColor
    }); 
}

void Engine::DebugSphere(const Vec3F& InPos, const float InRadius, const Rendering::Color InColor, const int InRings, const int InSlices)
{
    Engine::Instance::Get().GetRenderScene().AddDebugShape({
        Rendering::DebugShape::Type::SPHERE,
        InPos,
        QuatF::Identity(),
        Vec3F(
            InRadius,
            static_cast<float>(InRings),
            static_cast<float>(InSlices)),
        InColor
    }); 
}

void Engine::DebugBox(const Vec3F& InPos, const QuatF& InRot, const Vec3F& InExtent, const Rendering::Color InColor)
{
    Engine::Instance::Get().GetRenderScene().AddDebugShape({
        Rendering::DebugShape::Type::BOX,
        InPos,
        InRot,
        InExtent,
        InColor
    });
}

void Engine::DebugCapsule(const Vec3F& InPos, const QuatF& InRot, const float InRadius, const float InHeight, const Rendering::Color InColor, const int InSlices)
{
    Engine::Instance::Get().GetRenderScene().AddDebugShape({
        Rendering::DebugShape::Type::CAPSULE,
        InPos,
        InRot,
        Vec3F(
            InRadius,
            InHeight,
            static_cast<float>(InSlices)),
        InColor
    });
}
