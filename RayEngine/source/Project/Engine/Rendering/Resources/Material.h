#pragma once

#include "../../Property/Property.h"
#include "../../Property/PropertyOwner.h"
#include "Engine/Rendering/Resources/Shader.h"
#include "Engine/Resource/Resource.h"

class MaterialResource : public PropertyOwner<MaterialResource>
{
public:

    bool Load(const String& InPath) override;
    Utility::Timepoint GetEditTime() const; 
    
    PROPERTY_D(ResShader, SurfaceShader, "Defaults/SH_Default");
    PROPERTY_D(ResShader, DeferredShader, "Defaults/SH_Deferred_Default");
    PROPERTY_D(bool, TwoSided, false);

    // TODO: Material properties
    
    uint32 SurfaceHash() const;
    uint32 DeferredHash() const;
    
private:

    String Identifier; 
    
};

typedef Resource::Ref<MaterialResource, true> ResRM;  
