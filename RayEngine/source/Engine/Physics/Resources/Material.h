
#pragma once

#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Resource/Resource.h"

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
    physx::PxMaterial* Get() const { return Ptr; }
    
    PROPERTY_P(float, StaticFriction, 1.0f);
    PROPERTY_P(float, DynamicFriction, 1.0f);
    PROPERTY_P(float, Restitution, 1.0f);

private:

    String Identifier; 
    physx::PxMaterial* Ptr = nullptr;
    
};

typedef Resource::Ref<PhysicsMaterialResource> ResPM;  
