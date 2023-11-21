#pragma once

namespace Utility
{
    typedef size_t TypeHash; 
    
    class Type
    {
    public:
        Type() = default;
        Type(TypeHash InHash) : Hash(InHash) {}
        virtual ~Type() = default;

        template <class T>
        static Type Get()
        {
            return Type(typeid(T).hash_code());
        }

        // Convenience operators
        bool operator==(const Type& InOther) const { return InOther.Hash == Hash; }
        bool operator!() const { return *this == None(); }
        operator bool() const { return *this != None(); };
        operator TypeHash() const
        {
            return Hash;
        }
    
        TypeHash GetHash() const { return Hash; }
        static Type None() { return Type(); }
    
    private: 
        TypeHash Hash = 0; 
    };

    template <class T>
    Type GetType() { return Type::Get<T>(); }
}
