#pragma once

#include <numeric>

#include "Geometry/PlaneVolume.h"

namespace Utility
{
    namespace Math
    {

        // Returns all intersected grid positions
        template <class T = int>
        std::vector<Vector3<T>> GridIntersection(const Vec3F& rayOrigin, const Vec3F& rayDirection, Vector3<T> InBound)
        {
            // Create planes
            PlaneVolume<float> planeVolume;
            planeVolume.AddPlane(Plane({0.0f}, Vec3F::Up() * -1.0f));
            planeVolume.AddPlane(Plane({0.0f}, Vec3F::Right() * -1.0f));
            planeVolume.AddPlane(Plane({0.0f}, Vec3F::Forward() * -1.0f));
            Vec3F floatBound = {
                static_cast<float>(InBound.x),
                static_cast<float>(InBound.y),
                static_cast<float>(InBound.z) };
            planeVolume.AddPlane(Plane(floatBound, Vec3F::Up()));
            planeVolume.AddPlane(Plane(floatBound, Vec3F::Right()));
            planeVolume.AddPlane(Plane(floatBound, Vec3F::Forward()));
        
            // Find where in grid to start, clamp to inside grid
            Vec3F currPos = rayOrigin;
            planeVolume.FindIntersect(currPos, rayOrigin, rayDirection); 

            // How many steps of X Y Z per block? Sign is retained
            const float deltaX = 1.0f / rayDirection.x;
            const float deltaY = 1.0f / rayDirection.y;
            const float deltaZ = 1.0f / rayDirection.z;

            const auto sign = [](const float a) {
                return (a > 0.0f) ? 1 : ((a < 0.0f) ? -1 : 0);
            }; 
            int sx = sign(deltaX);
            int sy = sign(deltaY);
            int sz = sign(deltaZ);
            
            // Current coord 
            int x = static_cast<int>(currPos.x);
            int y = static_cast<int>(currPos.y);
            int z = static_cast<int>(currPos.z);

            // Calculate leftover delta?
            // We're not starting in the middle of a block
            // Diff between coord and pos
            Vec3F coordDiff = currPos - Vec3F(x, y, z);
            // We're already diff in
            // What is the leftover delta? 
            
            // Store leftover delta
            float dx = 0.0f;
            float dy = 0.0f;
            float dz = 0.0f;

            std::vector<Vector3<T>> result; 
            while (x >= 0 && x < InBound.x &&
                y >= 0 && y < InBound.y &&
                z >= 0 && z < InBound.z)
            {
                // Add position!
                result.push_back({
                    static_cast<T>(x),
                    static_cast<T>(y),
                    static_cast<T>(z)
                });
                
                // We want to step exactly the right amount that is needed to reach the next block
                // Add in direction of smallest of x,y,z
                // But also consider sign of delta... 
                
                if (dx < dy && dx < dz)
                {
                    const float d = deltaX * static_cast<float>(sx); 
                    dx += d;
                    x += d > 0.0f ? sx : -sx;
                }
                else if (dy < dz)
                {
                    const float d = deltaY * static_cast<float>(sy);
                    dy += d;
                    y += d > 0.0f ? sy : -sy;
                }
                else
                {
                    const float d = deltaZ * static_cast<float>(sz);
                    dz += d; 
                    z += d > 0.0f ? sz : -sz;
                }
            }
            return result; 
        };
    }
}
