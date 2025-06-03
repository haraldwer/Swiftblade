#pragma once
#include "Pointers.h"

namespace Utility
{
    // This scoped object can own an abstract type
    // And will be destroyed when out of scope
    // Basically just a normal object + inheritance!
    template <class BaseT>
    class Object 
    {
    public:

        Object() : Object(BaseT()) { }

        // Copy will create a new object
        Object(const Object& InOther) : Object(InOther.Get()) { }

        template <class T> 
        Object(const T& InData)
        {
            T* ptr = new T(InData);
            currentType = Type::Get<T>();
            ptr = reinterpret_cast<BaseT*>(ptr);
        }

        // Can be made into a weak ptr
        operator WeakPtr<BaseT>() const
        {
            return WeakPtr<BaseT>(ptr);
        }

        // A valid object is guaranteed
        template <class T = BaseT>
        T& Get()
        {
            CHECK_ASSERT(!IsA<T>(), "Invalid type");
            BaseT* ptr = ptr.Get();
            CHECK_ASSERT(!ptr, "Invalid ptr");
            return *reinterpret_cast<T*>(ptr); 
        }

        template <class T = BaseT>
        const T& Get() const
        {
            CHECK_ASSERT(!IsA<T>(), "Invalid type");
            BaseT* ptr = ptr.Get();
            CHECK_ASSERT(!ptr, "Invalid ptr");
            return *reinterpret_cast<T*>(ptr); 
        }

        template <class T = BaseT>
        operator T&() { return Get<T>(); }
        template <class T = BaseT>
        operator const T&() const { return Get<T>(); }
        
        template <class T = BaseT>
        T* operator->() { return &Get<T>(); }
        template <class T = BaseT>
        const T* operator->() const { return &Get<T>(); }
        
        template <class T>
        bool IsA()
        {
            const auto type = Type::Get<T>(); 
            return
                type == currentType ||
                type == Type::Get<BaseT>(); 
        }

    private:

        // Object will be automatically destroyed when out of scope
        ObjectPtr<BaseT> ptr;
        Type currentType; 
    };
}