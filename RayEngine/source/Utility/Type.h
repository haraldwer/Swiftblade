#ifndef TYPE_H
#define TYPE_H

//#pragma once

#include <typeinfo>

namespace Utility
{
    typedef size_t TypeHash; 
    
    class Type
    {
    public:
        Type() = default;
        explicit Type(TypeHash InHash) : Hash(InHash) {}
        virtual ~Type() = default;

        template <class T>
        static Type Get()
        {
            return Type(typeid(T).hash_code());
        }

        // Convenience operators
        bool operator==(const Type& InOther) const { return InOther.Hash == Hash; }
        bool operator!() const { return *this == None(); }
        explicit operator bool() const { return !(*this == None()); };
        explicit operator TypeHash() const
        {
            return Hash;
        }
    
        [[nodiscard]] TypeHash GetHash() const { return Hash; }
        static Type None() { return {}; }
    
    private: 
        TypeHash Hash = 0; 
    };
}

#endif