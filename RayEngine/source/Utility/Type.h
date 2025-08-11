#pragma once

#include <typeinfo>

namespace Utility
{
    typedef size_t TypeHash;
    
    class Type
    {
    public:
        Type() = default;
        explicit Type(const TypeHash InHash) : hash(InHash) {}
        virtual ~Type() = default;

        template <class T>
        static TypeHash GetHash()
        {
            return typeid(T).hash_code();
        }
        
        template <class T>
        static Type Get()
        {
            return Type(GetHash<T>());
        }
        
        // Convenience operators
        bool operator==(const Type& InOther) const { return InOther.hash == hash; }
        bool operator!() const { return *this == None(); }
        explicit operator bool() const { return !(*this == None()); };
        explicit operator TypeHash() const
        {
            return hash;
        }
    
        TypeHash GetHash() const { return hash; }
        static Type None() { return {}; }
    
    private: 
        TypeHash hash = 0; 
    };
}

#define TYPE_INFO(self, parent) \
public: \
    static Utility::Type GetType() { return Utility::Type::Get<self>(); } \
    static Utility::TypeHash GetTypeHash() { return Utility::Type::GetHash<self>(); } \
    static bool IsA(const Utility::Type& InType) { return InType == GetType() || parent::IsA(InType); } \
    static String TypeName() { return String(#self); } \
    static size_t TypeSize() { return sizeof(self); } \
    Utility::Type GetObjType() const override { return GetType(); } \
    String GetObjName() const override { return TypeName(); } \
private:

#define BASE_TYPE_INFO(self) \
public: \
    static Utility::Type GetType() { return Utility::Type::Get<self>(); } \
    static Utility::TypeHash GetTypeHash() { return Utility::Type::GetHash<self>(); } \
    static bool IsA(const Utility::Type& InType) { return InType == GetType(); } \
    static String TypeName() { return String(#self); } \
    static size_t TypeSize() { return sizeof(self); } \
    virtual Utility::Type GetObjType() const { return GetType(); } \
    virtual String GetObjName() const { return TypeName(); } \
private:
