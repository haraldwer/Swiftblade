#pragma once

class PropertyBase
{
public:
    virtual ~PropertyBase() = default;
    PropertyBase(const String& InName);
    const String& GetName() const { return Name; }
    
    virtual void Serialize(SerializeObj& InOutObj) const = 0;
    virtual void Deserialize(const DeserializeObj& InObj) = 0;
    
private:
    String Name; 
};