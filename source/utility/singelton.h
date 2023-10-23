#pragma once

#include "Utility/Utility.h"

namespace Utility
{
    template <class T>
    class Singelton
    {
    public:

        virtual ~Singelton() = default;
        Singelton()
        {
            CHECK_ASSERT(Instance, "Instance already set");
            Instance = reinterpret_cast<T*>(this); 
        }
        
        static T& Get()
        {
            CHECK_ASSERT(!Instance, "Instance invalid");
            return *Instance;
        }

        inline static T* Instance = nullptr;
    };
}
