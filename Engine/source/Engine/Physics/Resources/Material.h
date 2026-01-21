#pragma once

#include "Core/Resource/Resource.h"
#include "Core/Resource/PropertyFile.h"

struct PhysicsMaterialData : PropertyOwner<PhysicsMaterialData>
{
    PROPERTY_D(float, Friction, 1.0f);
    PROPERTY_D(float, Bounciness, 0.0f);
    PROPERTY_D(float, MassDensity, 1.0f);

    static bool Accept(const String& InPath)
    {
        return InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("PM_");
    }
};

typedef Resource::PropertyFile<PhysicsMaterialData> PhysicsMaterialResource;
typedef Resource::Ref<PhysicsMaterialResource> ResPM;  
