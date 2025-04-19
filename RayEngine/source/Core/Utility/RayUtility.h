#pragma once

#include "raylib.h"
#include "Rendering/Scene/Instances/CameraInstance.h"

namespace Utility
{
    namespace Ray
    {
        inline Vector3 ConvertVec(const Vec3F& InVec)
        {
            return { InVec.x, InVec.y, InVec.z };
        }

        inline Matrix ConvertMat(const Mat4F& InMat)
        {
            Matrix result = {};
            memcpy(&result.m0, InMat.data, sizeof(Mat4F));
            return result;
        }

        inline Mat4F ConvertBack(const Matrix& InMat)
        {
            Mat4F result = Mat4F();
            memcpy(result.data, &InMat.m0, sizeof(Mat4F));
            return result; 
        }

        inline ::Camera ConvertCamera(const CameraInstance& InCamera)
        {
            ::Camera cam;
            cam.fovy = InCamera.FOV;
            cam.position = ConvertVec(InCamera.Position);
            cam.projection = CAMERA_PERSPECTIVE;

            const Mat4F mat = Mat4F(InCamera.Position, InCamera.Rotation, Vec3F::One());
            cam.up = ConvertVec(mat.Up());
            cam.target = ConvertVec(InCamera.Position + mat.Forward());
            
            return cam; 
        }
    }
}
