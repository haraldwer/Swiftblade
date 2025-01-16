#pragma once

namespace ECS
{
    struct LookParams : PropertyOwner<LookParams>
    {
        PROPERTY_C(Vec2F, SensitivityMultiplier, Vec2F(1.0f)); 
    };

    struct MoveParams : PropertyOwner<MoveParams>
    {
        PROPERTY_C(float, InputDeadzone, 0.1f); 
        PROPERTY_C(float, MovementForce, 150.0f);
        Vec3F PlaneNormal = Vec3F::Zero();
    };

    struct JumpParams : PropertyOwner<JumpParams>
    {
        PROPERTY_C(float, UpVelocity, 22.0f);
        PROPERTY_C(float, DirectionalForce, 20.0f);
        Vec3F Direction = Vec3F::Zero();
    };

    struct SlowdownParams : PropertyOwner<SlowdownParams>
    {
        PROPERTY_C(float, Friction, 0.0009f); 
    };

    struct VelocityClampParams : PropertyOwner<VelocityClampParams>
    {
        PROPERTY_C(float, MaxSpeed, 12.0f);
        PROPERTY_C(float, MaxVerticalSpeed, 50.0f);
        PROPERTY_C(float, ClampSlowdown, 0.001f);
    };

    struct CrouchParams : PropertyOwner<CrouchParams>
    {
        PROPERTY_C(float, HeightMul, 0.01f); 
    };
}
