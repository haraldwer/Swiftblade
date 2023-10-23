#pragma once

#include "PropertyBase.h"
#include "Utility/Serialization.h"

#define PROPERTY(type, name, __VA_ARGS__) Property<type> name = Property<type>(#name, type(__VA_ARGS__));

template <class T>
class Property : public PropertyBase
{
public:
    Property(const String& InName, const T& InData) : PropertyBase(InName), Data(InData) {}
    operator T&() { return Data; }
    operator T() const { return Data; }

    String Serialize() const override
    {
        return Utility::Serialize(GetName(), Data);
    }
    
    void Deserialize(const String& InString) override
    {
        Utility::Deserialize(InString, GetName(), Data); 
    }
    
private: 
    T Data; 
};
