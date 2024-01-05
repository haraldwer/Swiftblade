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
            return Matrix(
                InMat.data[0],
                InMat.data[4],
                InMat.data[8],
                InMat.data[12],
                InMat.data[1],
                InMat.data[5],
                InMat.data[9],
                InMat.data[13],
                InMat.data[2],
                InMat.data[6],
                InMat.data[10],
                InMat.data[14],
                InMat.data[3],
                InMat.data[7],
                InMat.data[11],
                InMat.data[15]
                );
        }

        inline Mat4F ConvertBack(const Matrix& InMat)
        {
            Mat4F con = Mat4F();
            con.data[0] = InMat.m0;
            con.data[1] = InMat.m1;
            con.data[2] = InMat.m2;
            con.data[3] = InMat.m3;
            con.data[4] = InMat.m4;
            con.data[5] = InMat.m5;
            con.data[6] = InMat.m6;
            con.data[7] = InMat.m7;
            con.data[8] = InMat.m8;
            con.data[9] = InMat.m9;
            con.data[10] = InMat.m10;
            con.data[11] = InMat.m11;
            con.data[12] = InMat.m12;
            con.data[13] = InMat.m13;
            con.data[14] = InMat.m14;
            con.data[15] = InMat.m15;
            return con; 
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
