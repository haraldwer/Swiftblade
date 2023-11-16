#pragma once

namespace Utility
{
    typedef size_t TypeHash; 

    class TypeAny
    {
    public:
        TypeAny() = default;
        TypeAny(TypeHash InHash) : hash(InHash) {}
        virtual ~TypeAny() = default;

        // Convenience operators
        bool operator==(const TypeAny& InOther) const { return InOther.hash == hash; }
        bool operator!() const { return *this == None(); }
        operator bool() const { return *this != None(); };
    
        TypeHash GetHash() const { return hash; }
        static TypeAny None() { return TypeAny(); }
        
    protected: 
        TypeHash hash = 0; 
    };
    
    template <class T>
    class Type final : public TypeAny
    {
    public:
        Type() : TypeAny() { hash = typeid(T).hash_code(); }
        operator TypeAny() const { return TypeAny(hash); }
    };
    
    class TypedBase
    {
    public:
        virtual ~TypedBase() = default;
        virtual bool IsA(const TypeAny& InType) const { return false; }
    };
    
    template <class T>
    class Typed : TypedBase
    {
    public:
        Type<T> GetType() const { return Type<T>(); }
        bool IsA(const TypeAny& InType) const override { return InType == GetType(); }
    };

    template <class TFrom, class TTo>
    TTo* TypeCast(TFrom* InFrom)
    {
        if (InFrom && InFrom->IsA(TTo::GetType()))
            return reinterpret_cast<TTo*>(InFrom);
        return nullptr; 
    }
}
