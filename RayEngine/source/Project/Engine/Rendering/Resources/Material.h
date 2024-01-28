#pragma once

#include "..\..\Property\Property.h"
#include "..\..\Property\PropertyOwner.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Rendering/Resources/Shader.h"

class MaterialResource : public PropertyOwner<MaterialResource>
{
public:

    bool Load(const String& InPath) override;
    Utility::Timepoint GetEditTime() const; 
    
    PROPERTY_D(ResShader, Shader, "Defaults/SH_Default");
    PROPERTY_D(bool, TwoSided, false);

    // TODO: Material properties
    
private:

    String Identifier; 
    
};

typedef Resource::Ref<MaterialResource, true> ResRM;  
