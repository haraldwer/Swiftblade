#pragma once

#include "Core/Resource/Resource.h"
#include "Resource/PropertyFile.h"

struct PhysicsMaterialData : PropertyOwner<PhysicsMaterialData>
{
    PROPERTY_D(float, Friction, 1.0f);
    PROPERTY_D(float, Bounciness, 0.0f);
    PROPERTY_D(float, MassDensity, 1.0f);
};

typedef Resource::PropertyFile<PhysicsMaterialData> PhysicsMaterialResource;
typedef Resource::Ref<PhysicsMaterialResource> ResPM;  
