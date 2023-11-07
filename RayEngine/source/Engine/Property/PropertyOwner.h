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

    virtual void Serialize(SerializeObj& InOutObj) const;
    virtual bool Deserialize(const DeserializeObj& InObj);
    virtual bool Edit(); 

    virtual bool Save(const String& InPath) const;
    virtual bool Load(const String& InPath); 
    
private:
    
    inline static PropertyOwner* Instance = nullptr;
    Map<String, PropertyBase*> Properties;

    // TODO:
    // Instead of storing property pointer (that might be moved)
    // Store pointer offset compared to self
};
