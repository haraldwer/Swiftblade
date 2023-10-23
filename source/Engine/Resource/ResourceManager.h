#pragma once
#include "Utility/Singelton.h"

struct ResourceBase;

class ResourceManager : public Utility::Singelton<ResourceManager>
{
public:

    ResourceBase* GetResource(const String& InIdentifier)
    {
        return Resources[InIdentifier];
    }
    
    void Register(ResourceBase* InResource, const String& InIdentifier)
    {
        Resources[InIdentifier] = InResource;
    }

    // TODO: Unload

private:
    Map<String, ResourceBase*> Resources; 
};
