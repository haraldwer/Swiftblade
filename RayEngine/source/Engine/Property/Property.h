#pragma once

#include "PropertyBase.h"

// Defines a property
#define PROPERTY(type, name) Property<type> name = Property<type>(#name, type());

// Defines a property with a default value
#define PROPERTY_D(type, name, __VA_ARGS__) Property<type> name = Property<type>(#name, type(__VA_ARGS__));

// Defines a constant property, that can only be changed by Edit() and Deserialize()
#define PROPERTY_C(type, name, __VA_ARGS__) ConstantProperty<type> name = ConstantProperty<type>(#name, type(__VA_ARGS__));

template <class T>
class ConstantProperty : public PropertyBase
{
public:
    ConstantProperty(const String& InName, const T& InData) : PropertyBase(InName)
    {
        Data = InData;
        Default = InData;
    }
    
    void Reset() { Data = Default; }
    T GetDefault() const { return Default; }
    
    void Serialize(SerializeObj& InOutObj) const override
    {
        if (Data != Default)
            Utility::Serialize(InOutObj, GetName(), Data);
    }
    
    bool Deserialize(const DeserializeObj& InObj) override
    {
        return Utility::Deserialize(InObj, GetName(), Data); 
    }

    bool Edit(uint32 InOffset = 0) override
    {
        return Utility::Edit(GetName(), Data, InOffset); 
    }

    // Comparisons
    
    bool operator==(const PropertyBase& InOther) const override
    {
        return operator==(*reinterpret_cast<const ConstantProperty*>(&InOther));
    }
    
    bool operator==(const ConstantProperty& InOther) const
    {
        return Data == InOther.Data; 
    }

    // Getters
    operator const T&() const { return Data; }
    const T& Get() const { return Data; }
    
protected:

    T& GetData() { return Data; }
    const T& GetData() const { return Data; }
    
    T Data;
    T Default;
};

template <class T>
class Property : public ConstantProperty<T>
{
public:
    Property(const String& InName, const T& InData) : ConstantProperty<T>(InName, InData) {}

    // Non-const getters
    operator T&() { return this->Data; }
    operator const T&() const { return this->Data; }
    T& Get() { return this->Data; }
    const T& Get() const { return this->Data; }
    
    Property& operator = (const T& InData)
    {
        this->Data = InData;
        return *this;
    }
};