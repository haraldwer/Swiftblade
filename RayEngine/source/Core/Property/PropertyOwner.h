#pragma once

class PropertyBase;

class PropertyOwnerBase
{
public:
    
    virtual ~PropertyOwnerBase() = default;

    // Property registration
    static bool AddProperty(PropertyBase* InProperty);
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
    virtual Map<String, uint16>& GetPropertyMap() const = 0;
    virtual Vector<String>& GetPropertyOrder() const = 0;
    
    uint16 PtrToOff(PropertyBase* InPtr) const;
    PropertyBase* OffToPtr(uint16 InOff) const;

    // Current instance, only set during reg scope
    static inline PropertyOwnerBase* instance = nullptr;
    
private:
    
    bool AddPropertyInternal(PropertyBase* InProperty) const;
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

        // No recursion
        CHECK_RETURN(instance);

        // Allocate memory
        static TSelf local;
        instance = &local;

        // Run constructor to add properties
        new (&local) TSelf();
        
        // Reset instance
        instance = nullptr;

        // Now run constructor again to capture sub-types
        new (&local) TSelf();
    }

    size_t Size() const override { return sizeof(TSelf); }
    
private:

    Map<String, uint16>& GetPropertyMap() const override { return properties; }
    Vector<String>& GetPropertyOrder() const override { return propertyOrder; }
    
    // Store memory offset compared to (this)
    // Shared between all instances of the same type T
    static inline Map<String, uint16> properties;
    static inline Vector<String> propertyOrder;
    
};

namespace Utility
{
    template <class T>
    String ToStr(const PropertyOwner<T>& InValue)
    {
        rapidjson::StringBuffer s;
        rapidjson::Writer writer(s);
        InValue.Serialize(writer);
        return FormatJson(s.GetString());
    }
}