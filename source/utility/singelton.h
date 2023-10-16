#pragma once

#include "utility/utility.h"

template <class T>
class Singelton
{
    
public:

    virtual ~Singelton() = default; 

    virtual void Init()
    {
        Instance = static_cast<T*>(this); 
    }
    
    static T& Get()
    {
        CHECK_ASSERT(!Instance, "Instance invalid");
        return *Instance;
    }

    inline static T* Instance = nullptr;
    
};
