#pragma once

struct HeadState
{
    Vec3F Position;
    float Tilt = 0.0f;
    float Interp = 0.0f;

    void LerpTo(const HeadState& InTarget, float InDT)
    {
        if (InTarget.Interp < 0.01f)
        {
            *this = InTarget;
            return;
        }

        Interp = Utility::Math::Lerp(Interp, InTarget.Interp, InTarget.Interp * InDT);
        Tilt = Utility::Math::Lerp(Tilt, InTarget.Tilt, Interp * InDT);
        Position = Vec3F::Lerp(Position, InTarget.Position, Interp * InDT);
    }
};

enum class HandPose
{
    CLOSED,
    OPEN,
    POINTING // Could be cool?
};

struct HandState
{
    Mat4F Transform;
    HandPose Pose = HandPose::CLOSED;
    float Interp = 0.0f;
    float CameraSpace = 0.7f;
    Vec3F VelocityOffset = Vec3F(1.0f, 1.0f, 0.0f);
    
    void LerpTo(const HandState& InTarget, float InDt)
    {
        if (InTarget.Interp < 0.01f)
        {
            *this = InTarget;
            return;
        }

        Interp = Utility::Math::Lerp(Interp, InTarget.Interp, InTarget.Interp * InDt);
        Transform = Mat4F::Lerp(Transform, InTarget.Transform, Interp * InDt);
        CameraSpace = Utility::Math::Lerp(CameraSpace, InTarget.CameraSpace, Interp * InDt); 
    }
};