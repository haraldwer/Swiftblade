#pragma once

struct LightInstance
{
    struct InstanceData
    {
        Vec3F position = {};
        Vec3F direction = {};
        Vec3F color = {};
        float range = {};
        float radius = {};
        float intensity = {};
        
        bool operator==(const InstanceData& InData) const
        {
            return
                position == InData.position &&
                direction == InData.direction &&
                color == InData.color &&
                abs(range - InData.range) < 0.1f &&
                abs(radius - InData.radius) < 0.1f &&
                abs(intensity - InData.intensity) < 0.1f;
        }
    } data = {};
    
    bool shadows = false;
    uint32 id = 0;
};
