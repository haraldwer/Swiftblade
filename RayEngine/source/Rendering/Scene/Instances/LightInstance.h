#pragma once

struct LightInstance
{
    struct InstanceData
    {
        Vec3F Position;
        Vec3F Direction;
        Vec3F Color;
        float Range;
        float Radius;
        float Intensity;
        
        bool operator==(const InstanceData& InData) const
        {
            return
                Position == InData.Position &&
                Direction == InData.Direction &&
                Color == InData.Color &&
                abs(Range - InData.Range) < 0.1f &&
                abs(Radius - InData.Radius) < 0.1f &&
                abs(Intensity - InData.Intensity) < 0.1f;
        }
    } Data;
    
    bool Shadows = false;
    uint32 ID = 0;
};
