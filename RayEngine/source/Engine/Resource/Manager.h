#pragma once
#include "Utility/Singelton.h"

namespace Resource
{
    struct Base;
    
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:

        Base* GetResource(const String& InIdentifier);
        void Register(Base* InResource, const String& InIdentifier);
        
        void Update() const;
        void Deinit();

    private:
        Map<String, Base*> Resources; 
    };
}
