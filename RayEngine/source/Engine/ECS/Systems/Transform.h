#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Transform : Component<Transform>
    {
        PROPERTY(Vec3F, Position);
        PROPERTY(QuatF, Rotation);
        PROPERTY_P(Vec3F, Scale, Vec3F::One());
        PROPERTY_P(bool, Static, false); 

        // Parent?
        // Children?

        Mat4F Matrix() const
        {
            return Mat4F(
                Position,
                Rotation,
                Scale);
        }
    };

    class SysTransform : public System<Transform>
    {
    };
}

