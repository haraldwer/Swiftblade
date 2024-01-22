#pragma once

class PropertyBase
{
public:
    virtual ~PropertyBase() = default;
    PropertyBase(const String& InName);
    const String& GetName() const;

    virtual void Serialize(SerializeObj& InOutObj) const = 0;
    virtual bool Deserialize(const DeserializeObj& InObj) = 0;
    virtual bool Edit(uint32 InOffset) = 0;
    
    virtual bool operator==(const PropertyBase& InOther) const;

private:
    String Name; // Should the property know about its own name?  
};