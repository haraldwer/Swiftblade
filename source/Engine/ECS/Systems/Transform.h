#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Transform : Component
    {
        PROPERTY(Vec3F, Position); 
        PROPERTY(QuatF, Rotation);
        PROPERTY_P(Vec3F, Scale, Vec3F::One());

        // Parent?
        // Children?

        Mat4F Matrix() const
        {
            return Mat4F(
                Rotation,
                Position,
                Scale);
        }
    };

    class SysTransform : public System<Transform>
    {
    };
}

