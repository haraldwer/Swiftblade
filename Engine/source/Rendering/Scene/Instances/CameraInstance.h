#pragma once

namespace Rendering
{
    struct CameraInstance
    {
        Vec3F position;
        QuatF rotation;
        float fov = 90.0f;
        float far = 50.0f;
        float near = 0.01f;

        Mat4F GetViewMatrix() const
        {
            return Mat4F(position, rotation, Vec3F::One());
        }
        
        Mat4F GetProjectionMatrix(const Vec2F &InSize) const
        {
            Mat4F proj = Mat4F();
            const float aspect = InSize.x / InSize.y;

            float fovRad = Utility::Math::DegreesToRadians(fov);
            float f = 1.0f / tan(fovRad / 2.0f);
            proj(0, 0) = f / aspect;
            proj(1, 1) = f;
            proj(2, 2) = (far + near) / (near - far);
            proj(3, 2) = (2.0f * far * near) / (near - far);
            proj(2, 3) = -1.0f;
            proj(3, 3) = 0.0f;

            //proj.forward *= -1.0f;
            //proj.right *= -1.0f;
            
            return proj;
        }

        Array<Vec3F, 8> GetFrustumCorners(const Vec2F& InSize) const
        {
            // Calculate frustum edge points
            // For each point, ask if on left or right side
            
            const Vec3F v = rotation.ForwardDirection();
            const Vec3F up = rotation.UpDirection();
            const Vec3F w = rotation.RightDirection();
	    
            // First we will get the width and height of the near plane
            const float ar = InSize.x / InSize.y;
            const float fovRad = Utility::Math::DegreesToRadians(fov);
            
            const float Hnear = 2.0f * tanf(fovRad / 2.0f) * near;
            const float Wnear = Hnear * ar;

            // Then we do the same for the far plane
            const float Hfar = 2.0f * tanf(fovRad / 2.0f) * far;
            const float Wfar = Hfar * ar;

            // Now we get the center of the planes
            const Vec3F Cnear = position + v * near;
            const Vec3F Cfar = position + v * far;

            const Vec3F NearTopLeft = Cnear + (up * (Hnear / 2)) - (w * (Wnear / 2));
            const Vec3F NearTopRight = Cnear + (up * (Hnear / 2)) + (w * (Wnear / 2));
            const Vec3F NearBottomLeft = Cnear - (up * (Hnear / 2)) - (w * (Wnear /2));
            const Vec3F NearBottomRight = Cnear - (up * (Hnear / 2)) + (w * (Wnear / 2));
            const Vec3F FarTopLeft = Cfar + (up * (Hfar / 2)) - (w * (Wfar / 2));
            const Vec3F FarTopRight = Cfar + (up * (Hfar / 2)) + (w * (Wfar / 2));
            const Vec3F FarBottomLeft = Cfar - (up * (Hfar / 2)) - (w * (Wfar / 2));
            const Vec3F FarBottomRight = Cfar - (up * (Hfar / 2)) + (w * (Wfar / 2));

            return {
                NearTopLeft,
                NearTopRight,
                NearBottomLeft,
                NearBottomRight,
                FarTopLeft,
                FarTopRight,
                FarBottomLeft,
                FarBottomRight
            };
        }
    };
}
