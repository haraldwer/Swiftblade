#pragma once

#include "raylib.h"

#include "Engine/Rendering/Instances/CameraInstance.h"

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
            // Maybe flip row / colum
            return Matrix(
                InMat.data[0],
                InMat.data[1],
                InMat.data[2],
                InMat.data[3],
                InMat.data[4],
                InMat.data[5],
                InMat.data[6],
                InMat.data[7],
                InMat.data[8],
                InMat.data[9],
                InMat.data[10],
                InMat.data[11],
                InMat.data[12],
                InMat.data[13],
                InMat.data[14],
                InMat.data[15]
                );
        }

        inline Camera ConvertCamera(const Rendering::CameraInstance& InCamera)
        {
            Camera cam;
            cam.fovy = InCamera.FOV;
            cam.position = ConvertVec(InCamera.Position);
            cam.projection = CAMERA_PERSPECTIVE;
            cam.up = ConvertVec(Vec3F::Up());
            cam.target = ConvertVec(InCamera.Direction);
            return cam; 
        }

    }
}
