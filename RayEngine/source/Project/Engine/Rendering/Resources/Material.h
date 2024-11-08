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
    
    PROPERTY_D(ResShader, SurfaceShader, "Shaders/SH_Default");
    PROPERTY_D(ResShader, DeferredShader, "Shaders/SH_Default.ds");
    PROPERTY_D(bool, TwoSided, false);

    // TODO: Material properties
    
    uint32 SurfaceHash() const;
    uint32 DeferredHash() const;
    
private:

    String Identifier; 
    
};

typedef Resource::Ref<MaterialResource, true> ResRM;  
