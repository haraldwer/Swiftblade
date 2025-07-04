#pragma once

#include "Core/Resource/Resource.h"

namespace physx
{
    class PxMaterial;
}

class PhysicsMaterialResource : public PropertyOwner<PhysicsMaterialResource>
{
public:

    bool Load(const String& InPath) override;
    bool Unload() override;
    Utility::Timepoint GetEditTime() const; 
    physx::PxMaterial* Get() const { return ptr; }
    
    PROPERTY_D(float, StaticFriction, 1.0f);
    PROPERTY_D(float, DynamicFriction, 1.0f);
    PROPERTY_D(float, Restitution, 0.5f);

    // Potentially load with overrides

private:

    String identifier = {}; 
    physx::PxMaterial* ptr = nullptr;
    
};

typedef Resource::Ref<PhysicsMaterialResource, true> ResPM;  
