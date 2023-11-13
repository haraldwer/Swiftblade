#pragma once
#include "Utility/Singelton.h"
#include "Utility/Time/Timer.h"

namespace Resource
{
    struct Base;
    
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:

        Base* GetResource(const String& InIdentifier);
        void Register(Base* InResource, const String& InIdentifier);
        
        void Update();
        void Deinit();

    private:
        Map<String, Base*> Resources;
        Utility::Timer CheckTimer; 
    };
}
