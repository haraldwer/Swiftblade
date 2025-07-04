#pragma once

#include "PropertyBase.h"

// Defines a property
#define PROPERTY(type, name) Property<type> name = Property<type>(#name, type());

// Specific implementation for enums 
#define PROPERTY_E(type, name, __VA_ARGS__) EnumProperty<type> name = EnumProperty<type>(#name, type(__VA_ARGS__));

// Defines a property with a default value
#define PROPERTY_D(type, name, __VA_ARGS__) Property<type> name = Property<type>(#name, { __VA_ARGS__ });

// Defines a constant property, that can only be changed by Edit() and Deserialize()
#define PROPERTY_C(type, name, __VA_ARGS__) ConstantProperty<type> name = ConstantProperty<type>(#name, type(__VA_ARGS__));

template <class T>
class ConstantProperty : public PropertyBase
{
public:
    ConstantProperty(const String& InName, const T& InData) : PropertyBase(InName), data(InData), defaultValue(InData) { }
    
    void Reset() { data = defaultValue; }
    T GetDefault() const { return defaultValue; }
    
    void Serialize(SerializeObj& InOutObj) const override
    {
        if (data != defaultValue)
            Utility::Serialize(InOutObj, GetName(), data);
    }
    
    bool Deserialize(const DeserializeObj& InObj) override
    {
        return Utility::Deserialize(InObj, GetName(), data); 
    }

    bool Edit(uint32 InOffset = 0) override
    {
        return Utility::Edit(GetName(), data, InOffset); 
    }
    
    // Comparisons
    
    bool operator==(const PropertyBase& InOther) const override
    {
        return operator==(*reinterpret_cast<const ConstantProperty*>(&InOther));
    }
    
    bool operator==(const ConstantProperty& InOther) const
    {
        return data == InOther.data; 
    }

    // Getters
    operator const T&() const { return data; }
    const T& Get() const { return data; }
    
protected:
    
    T& GetData() { return data; }
    const T& GetData() const { return data; }
    
    T data;
    T defaultValue;
};

template <class T>
class Property : public ConstantProperty<T>
{
public:
    Property(const String& InName, const T& InData) : ConstantProperty<T>(InName, InData) {}

    // Non-const getters
    operator T&() { return this->data; }
    operator const T&() const { return this->data; }
    T& Get() { return this->data; }
    const T& Get() const { return this->data; }
    
    Property& operator = (const T& InData)
    {
        this->data = InData;
        return *this;
    }
};

template <class T>
class EnumProperty : public Property<T>
{
public:
    EnumProperty(const String& InName, const T& InData) : Property<T>(InName, InData) {}

    void Serialize(SerializeObj& InOutObj) const override
    {
        if (this->data != this->defaultValue)
        {
            const int data = static_cast<int>(this->data);
            Utility::Serialize(InOutObj, this->GetName(), data);
        }
    }
    
    bool Deserialize(const DeserializeObj& InObj) override
    {
        int data = static_cast<int>(this->data);
        if (Utility::Deserialize(InObj, this->GetName(), data))
        {
            this->data = static_cast<T>(data);
            return true;
        }
        return false;
    }

    bool Edit(const uint32 InOffset = 0) override
    {
        int data = static_cast<int>(this->data);
        if (Utility::Edit(this->GetName(), data, InOffset))
        {
            this->data = static_cast<T>(data);
            return true;
        }
        return false; 
    }
};