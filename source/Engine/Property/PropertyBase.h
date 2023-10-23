#pragma once

class PropertyBase
{
public:
    virtual ~PropertyBase() = default;
    PropertyBase(const String& InName);
    const String& GetName() const { return Name; }
    
    virtual String Serialize() const = 0;
    virtual void Deserialize(const String& InString) = 0;
    
private:
    String Name; 
};