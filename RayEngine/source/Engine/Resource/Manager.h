#pragma once
#include "Utility/Singelton.h"

namespace Resource
{
    struct Base;
    
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:

        Base* GetResource(const String& InIdentifier)
        {
            const auto find = Resources.find(InIdentifier);
            if (find == Resources.end())
                return nullptr; 
            return find->second;
        }
        
        void Register(Base* InResource, const String& InIdentifier)
        {
            Resources[InIdentifier] = InResource;
        }
        
        // TODO: Unload

        
        void Deinit()
        {
            // TODO: Unload all
        }

    private:
        Map<String, Base*> Resources; 
    };
}
