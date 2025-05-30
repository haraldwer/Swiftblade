#pragma once

struct CameraInstance
{
    Vec3F Position;
    QuatF Rotation;
    float FOV;
    float Far;
    float Near;

    Mat4F GetViewMatrix() const
    {
        return Mat4F(Position, Rotation, Vec3F::One());
    }
    
    Mat4F GetProjectionMatrix(Vec2F InSize) const
    {
        Mat4F proj = Mat4F();
        const float aspect = InSize.x / InSize.y;

        float fovRad = Utility::Math::DegreesToRadians(FOV);
        float f = 1.0f / tan(fovRad / 2.0f);
        proj(0, 0) = f / aspect;
        proj(1, 1) = f;
        proj(2, 2) = (Far + Near) / (Near - Far);
        proj(3, 2) = (2.0f * Far * Near) / (Near - Far);
        proj(2, 3) = -1.0f;
        proj(3, 3) = 0.0f;

        proj.forward *= -1.0f;
        proj.right *= -1.0f;
        
        return proj;
    }
};