#pragma once

#include "Engine/Property/Property.h"
#include "..\..\Property\PropertyOwner.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Rendering/Resources/Shader.h"

class MaterialResource : public PropertyOwner<MaterialResource>
{
public:

    bool Load(const String& InPath) override;
    bool Unload();
    Material* Get() const { return Ptr; }
    
    PROPERTY(ResShader, Shader);
    // Shader properties? 
    // Other stuff? 
    
private:

    Material* Ptr = nullptr; 
    
};

typedef Resource::Ref<MaterialResource> ResRM;  
