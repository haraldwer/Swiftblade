#pragma once

class PropertyBase;

class PropertyOwnerBase
{
public:
    
    virtual ~PropertyOwnerBase() = default;

    // Property registration
    static void AddProperty(PropertyBase* InProperty);
    Map<String, PropertyBase*> GetProperties() const;
    virtual size_t Size() const = 0;

    virtual void CustomSerialize(SerializeObj& InOutObj) const {}
    virtual bool CustomDeserialize(const DeserializeObj& InObj) { return true; }
    virtual bool Edit(const String& InName = "", uint32 InOffset = 0);
    
    void Serialize(SerializeObj& InOutObj) const;
    bool Deserialize(const DeserializeObj& InObj);
    bool Deserialize(const GenericVal& InVal);

    virtual bool Save(const String& InPath) const;
    virtual bool Load(const String& InPath);
    virtual bool Unload() { return true; }

    bool operator==(const PropertyOwnerBase& InOther) const;

protected:

    // Property map owned by template  
    virtual OrderedMap<String, uint16>& GetPropertyMap() const = 0;
    uint16 PtrToOff(PropertyBase* InPtr) const;
    PropertyBase* OffToPtr(uint16 InOff) const;

    // Current instance, only set during reg scope
    static inline PropertyOwnerBase* Instance = nullptr;
    
private:
    
    void AddPropertyInternal(PropertyBase* InProperty) const;
};

template <class TSelf>
class PropertyOwner : public PropertyOwnerBase
{
public:

    ~PropertyOwner() override = default;
    PropertyOwner()
    {
        Reg(); 
    }
    
    static void Reg()
    {
        // Only do once
        static bool hasRegistered = false;
        CHECK_RETURN(hasRegistered); 
        hasRegistered = true;
        
        // Cache instance, use recursion as stack 
        PropertyOwnerBase* prevInstance = Instance;

        // Allocate memory
        static TSelf local;
        Instance = &local;
        
        // Create new copy
        // Will run constructor
        // And properties will be added
        new (&local) TSelf();
        
        // Reset instance
        Instance = prevInstance;
    }

    size_t Size() const override { return sizeof(TSelf); }
    
private:

    OrderedMap<String, uint16>& GetPropertyMap() const override { return Properties; }
    
    // Store memory offset compared to (this)
    // Shared between all instances of the same type T
    static inline OrderedMap<String, uint16> Properties;
    
};