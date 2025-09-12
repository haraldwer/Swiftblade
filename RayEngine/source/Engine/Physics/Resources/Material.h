#pragma once

#include "Core/Resource/Resource.h"
#include "Resource/PropertyFile.h"

struct PhysicsMaterialData : PropertyOwner<PhysicsMaterialData>
{
    PROPERTY_D(float, StaticFriction, 1.0f);
    PROPERTY_D(float, DynamicFriction, 1.0f);
    PROPERTY_D(float, Restitution, 0.5f);
};

class PhysicsMaterialResource : public Resource::PropertyFile<PhysicsMaterialData>
{
public:
    bool Load() override;
    bool Unload() override; 
    void* Get() const { return ptr; }
    
    // Potentially load with overrides

private:

    String identifier = {}; 
    void* ptr = nullptr;
    
};

typedef Resource::Ref<PhysicsMaterialResource> ResPM;  
