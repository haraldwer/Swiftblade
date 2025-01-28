#pragma once

#include "Geometry/PlaneVolume.h"

namespace Utility
{
    namespace Math
    {

        // Returns all intersected grid positions
        template <class T = int>
        std::vector<Vector3<T>> GridIntersection(const Vec3F& rayOrigin, const Vec3F& rayDirection, float InTraceDist)
        {
            // Börja med utgångsposition
            // Ta ett steg i någon riktning
            // Vilken riktning? Hur stort är steget?
            // Vi vill minsta distans för att komma in i nästa ruta

            // dx, dy är rayDirection
            // För att gå en ruta i X måste man gå dx * l -> 1. Längden man måste gå för en ruta är alltså 1 / dx -> l
            // Samma för y och z
            
            float l = 0;
            Vec3F sx = Vec3F::One() / (rayDirection + 0.0000001f); 
            
            std::vector<Vector3<T>> result;
            while (l < InTraceDist)
            {
                Vec3F pos = rayOrigin + rayDirection * l;
                
                Vec3F round = Vec3F(
                    std::round(pos.x),
                    std::round(pos.y),
                    std::round(pos.z));


                Vector3<T> r = {
                    static_cast<T>(round.x),
                    static_cast<T>(round.y),
                    static_cast<T>(round.z)
                };
                if (result.empty() || result.back() != r)
                    result.push_back(r);
                
                Vec3F diff = round - pos;
                Vec3F distance = diff * sx; // How far along l to closest intersect

                // Already on this intersect, go to next intersect
                if (distance.x < 0.00001f)
                    distance.x += abs(sx.x);
                if (distance.y < 0.00001f)
                    distance.y += abs(sx.y);
                if (distance.z < 0.00001f)
                    distance.z += abs(sx.z);
                
                // select smallest direction
                l += Min(Min(distance.x, distance.y), distance.z) + 0.001f;
            }
            return result;
        };
    }
}
