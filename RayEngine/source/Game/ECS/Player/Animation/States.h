#pragma once

struct HeadState
{
    Vec3F Position;
    float Tilt = 0.0f;
    float Interp = 0.0f;
};

enum class HandPose
{
    CLOSED,
    OPEN,
    POINTING // Could be cool?
};

struct HandState
{
    Vec3F Position;
    QuatF Rotation;
    HandPose Pose = HandPose::CLOSED;
    float Interp = 0.0f;
    float CameraSpace = 0.7f;
    
    void LerpTo(const HandState& InTarget, float InDt)
    {
        if (InTarget.Interp < 0.01f)
        {
            *this = InTarget;
            return;
        }

        Interp = Utility::Math::Lerp(Interp, InTarget.Interp, InTarget.Interp * InDt);
        Position = Utility::Math::Lerp(Position, InTarget.Position, Interp * InDt);
        Rotation = QuatF::Slerp(Rotation, InTarget.Rotation, Interp * InDt);
        CameraSpace = Utility::Math::Lerp(CameraSpace, InTarget.CameraSpace, Interp * InDt); 
    }
};