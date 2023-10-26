#pragma once

#include "PropertyBase.h"
#include "Utility/Serialization.h"

#define PROPERTY(type, name) Property<type> name = Property<type>(#name, type());
#define PROPERTY_P(type, name, __VA_ARGS__) Property<type> name = Property<type>(#name, type(__VA_ARGS__));

template <class T>
class Property : public PropertyBase
{
public:
    Property(const String& InName, const T& InData) : PropertyBase(InName), Data(InData), Default(InData) {}
    operator T&() { return Data; }
    operator T() const { return Data; }
    Property& operator = (const T& InData)
    {
        Data = InData;
        return *this;
    }

    void Reset() { Data = Default; }
    T GetDefault() const { return Default; }
    
    void Serialize(SerializeObj& InOutObj) const override
    {
        // TODO: Compare against default value
        Utility::Serialize(InOutObj, GetName(), Data);
    }
    
    void Deserialize(const DeserializeObj& InObj) override
    {
        Utility::Deserialize(InObj, GetName(), Data); 
    }
    
private: 
    T Data;
    T Default;
};
