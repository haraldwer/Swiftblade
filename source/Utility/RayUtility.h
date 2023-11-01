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
            const Mat4F con = InMat;
            
            return Matrix(
                con.data[0],
                con.data[4],
                con.data[8],
                con.data[12],
                con.data[1],
                con.data[5],
                con.data[9],
                con.data[13],
                con.data[2],
                con.data[6],
                con.data[10],
                con.data[14],
                con.data[3],
                con.data[7],
                con.data[11],
                con.data[15]
                );
        }

        inline Camera ConvertCamera(const CameraInstance& InCamera)
        {
            Camera cam;
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
