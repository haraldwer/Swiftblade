#pragma once

#include "Utility/Utility.h"

namespace Utility
{
    struct SingeltonContext
    {
        inline static int Value = -1; 
    };

    template <class T, bool Global = false>
    class Singelton
    {
    public:
        virtual ~Singelton()
        {
            Singelton*& ptr = Global ?
                instance : contextInstance[SingeltonContext::Value];
            ptr = nullptr;
        }

        Singelton()
        {
            Singelton*& ptr = Global ?
                instance : contextInstance[SingeltonContext::Value];
            CHECK_ASSERT(ptr, "Instance already set");
            ptr = this;
        }

        static T& Get()
        {
            Singelton*& ptr = Global ?
                instance : contextInstance[SingeltonContext::Value];
            CHECK_ASSERT(!ptr, "Instance invalid");
            return *reinterpret_cast<T*>(ptr);
        }
        
        inline static Singelton* instance = nullptr;
        inline static Map<int, Singelton*> contextInstance;
    };
}
