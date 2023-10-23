#pragma once

class PropertyBase;

class PropertyOwner
{
public:
    virtual ~PropertyOwner() = default;
    PropertyOwner() { StartScope(); }
    void StartScope();
    void EndScope(); // TODO: End scope!
    static void AddProperty(PropertyBase* InProperty);

    // User can implement custom serialization functions
    virtual String Serialize() const;
    virtual void Deserialize(const String& InString);
    
private:
    
    inline static Vector<PropertyOwner*> InstanceStack;
    Map<String, PropertyBase*> Properties;

    // TODO:
    // Instead of storing property pointer (that might be moved)
    // Store pointer offset compared to self
};
