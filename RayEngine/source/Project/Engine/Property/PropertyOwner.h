#pragma once

class PropertyBase;

class PropertyOwnerBase
{
public:
    
    virtual ~PropertyOwnerBase() = default;

    // Property registration
    static void AddProperty(PropertyBase* InProperty);
    Map<String, PropertyBase*> GetProperties() const;

    virtual void Serialize(SerializeObj& InOutObj) const;
    virtual bool Deserialize(const DeserializeObj& InObj);
    bool Deserialize(const GenericVal& InVal);
    virtual bool Edit(const String& InName = ""); 

    virtual bool Save(const String& InPath) const;
    virtual bool Load(const String& InPath);
    virtual bool Unload() { return true; }

    bool operator==(const PropertyOwnerBase& InOther) const;

protected:

    // Property map owned by template  
    virtual Map<String, uint16>& GetPropertyMap() const = 0; 
    uint16 PtrToOff(PropertyBase* InPtr) const;
    PropertyBase* OffToPtr(uint16 InOff) const;

    // Current instance, only set during reg scope
    static inline PropertyOwnerBase* Instance = nullptr;
    
private:
    
    void AddPropertyInternal(PropertyBase* InProperty) const;
};

template <class T>
class PropertyOwner : public PropertyOwnerBase
{

public:

    PropertyOwner()
    {
        Reg(); 
    }
    
    static void Reg()
    {
        CHECK_RETURN(Instance);
        
        // Only do once
        static bool hasRegistered = false;
        CHECK_RETURN(hasRegistered); 
        hasRegistered = true; 
        
        // Local T instance 
        T instance;
        
        // Set instance
        PropertyOwnerBase* prevInstance = Instance; 
        Instance = &instance;
        
        // Create new copy
        // Will run constructor
        // And properties will be added
        instance.~T();
        new (&instance) T();
        
        // Reset instance 
        Instance = prevInstance;
    }
    
private:

    Map<String, uint16>& GetPropertyMap() const override { return Properties; }
    
    // Store memory offset compared to (this)
    // Shared between all instances of the same type T
    static inline Map<String, uint16> Properties;
    
};