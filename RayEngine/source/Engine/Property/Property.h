#pragma once

#include "PropertyBase.h"
#include "Utility/Serialization/Serialize.h"
#include "Utility/Serialization/Deserialize.h"
#include "Utility/Serialization/Edit.h"

#define PROPERTY(type, name) Property<type> name = Property<type>(#name, type());
#define PROPERTY_P(type, name, __VA_ARGS__) Property<type> name = Property<type>(#name, type(__VA_ARGS__));

template <class T>
class Property : public PropertyBase
{
public:
    Property(const String& InName, const T& InData) : PropertyBase(InName) { Data = InData, Default = InData; }
    operator T&() { return Data; }
    operator const T&() const { return Data; }
    T& Get() { return Data; }
    const T& Get() const { return Data; }
    Property& operator = (const T& InData)
    {
        Data = InData;
        return *this;
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

    bool Edit() override
    {
        return Utility::Edit(GetName(), Data); 
    }

    bool operator==(const PropertyBase& InOther) const override
    {
        // Assuming type of InOther
        return operator==(*reinterpret_cast<const Property*>(&InOther));
    }
    
    bool operator==(const Property& InOther) const
    {
        return Data == InOther.Data; 
    }

private: 
    T Data;
    T Default;
};
