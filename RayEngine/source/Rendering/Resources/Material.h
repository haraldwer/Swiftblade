#pragma once

#include "Rendering/Resources/Shader.h"
#include "Core/Resource/Resource.h"

class MaterialResource : public PropertyOwner<MaterialResource>
{
public:

    bool Load(const String& InPath) override;
    Utility::Timepoint GetEditTime() const; 
    
    PROPERTY_D(ResShader, SurfaceShader, "Shaders/SH_Default");
    PROPERTY_D(ResShader, DeferredShader, "Shaders/SH_Default.ds");
    PROPERTY_D(bool, TwoSided, false);
    PROPERTY_D(bool, Transparent, false);

    // TODO: Material properties
    
    uint32 Hash();
    uint32 DeferredHash();
    
private:

    uint32 CachedHash = 0;
    uint32 CachedDeferredHash = 0;
    String Identifier = {}; 
    
};

typedef Resource::Ref<MaterialResource, true> ResRM;  
