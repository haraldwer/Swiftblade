#pragma once

struct HeadState
{
    Vec3F position;
    float tilt = 0.0f;
    float interp = 0.0f;

    void LerpTo(const HeadState& InTarget, float InDT)
    {
        if (InTarget.interp < 0.01f)
        {
            *this = InTarget;
            return;
        }

        interp = Utility::Math::Lerp(interp, InTarget.interp, InTarget.interp * InDT);
        tilt = Utility::Math::Lerp(tilt, InTarget.tilt, interp * InDT);
        position = Vec3F::Lerp(position, InTarget.position, interp * InDT);
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
    Mat4F transform;
    HandPose pose = HandPose::CLOSED;
    float interp = 0.0f;
    float cameraSpace = 0.7f;
    Vec3F velocityOffset = Vec3F(1.0f, 1.0f, 0.0f);
    
    void LerpTo(const HandState& InTarget, const float InDt)
    {
        if (InTarget.interp < 0.01f)
        {
            *this = InTarget;
            return;
        }

        interp = Utility::Math::Lerp(interp, InTarget.interp, InTarget.interp * InDt);
        transform = Mat4F::Lerp(transform, InTarget.transform, interp * InDt);
        cameraSpace = Utility::Math::Lerp(cameraSpace, InTarget.cameraSpace, interp * InDt); 
    }
};