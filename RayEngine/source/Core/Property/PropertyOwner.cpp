#include "PropertyBase.h"
#include "PropertyOwner.h"

bool PropertyOwnerBase::AddProperty(PropertyBase* InProperty)
{
    CHECK_RETURN(!instance, false);
    CHECK_ASSERT(!InProperty, "Property null");
    return instance->AddPropertyInternal(InProperty);
}

Map<String, PropertyBase*> PropertyOwnerBase::GetProperties() const
{
    Map<String, PropertyBase*> result; 
    for (const auto& p : GetPropertyMap())
        result[p.first] = OffToPtr(p.second);
    return result; 
}

void PropertyOwnerBase::Serialize(SerializeObj& InOutObj) const
{
    InOutObj.StartObject();
    for (const auto& p : GetPropertyMap())
    {
        const PropertyBase* ptr = OffToPtr(p.second);
        CHECK_ASSERT(!ptr, "Invalid property");
        ptr->Serialize(InOutObj);
    }
    CustomSerialize(InOutObj);
    InOutObj.EndObject();
}

bool PropertyOwnerBase::Deserialize(const DeserializeObj& InObj)
{
    bool success = true;
    for (const auto& p : GetPropertyMap())
    {
        PropertyBase* ptr = OffToPtr(p.second);
        CHECK_ASSERT(!ptr, "Invalid property");
        if (!ptr->Deserialize(InObj))
            success = false;
    }
    if (!CustomDeserialize(InObj))
        success = false;
    return success;
}

bool PropertyOwnerBase::Deserialize(const GenericVal& InVal)
{
    CHECK_RETURN_LOG(!InVal.IsObject(), "Incorrect type, expected object", false);
    return Deserialize(InVal.GetObj());
}

bool PropertyOwnerBase::Edit(const String& InName, const uint32 InOffset)
{
    bool edited = false;
    const auto& order = GetPropertyOrder();
    const auto& map = GetPropertyMap();
    bool header = false;
    if (order.size() <= 1 || Utility::MaybeCollapse(InName, InOffset, header))
    {
        if (header)
            Utility::Indent();
        for (const auto& p : order)
        {
            const uint16 pOff = map.at(p);
            PropertyBase* ptr = OffToPtr(pOff);
            CHECK_ASSERT(!ptr, "Invalid property");
            if (ptr->Edit(InOffset + static_cast<uint32>(pOff)))
                edited = true;
        }
        if (header)
            Utility::Unindent();
    }
    return edited;
}

String PropertyOwnerBase::ToStr(const bool InFormat) const
{
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    Serialize(writer);
    String str = s.GetString();
    return InFormat ? Utility::FormatJson(str) : str;
}

bool PropertyOwnerBase::Save(const String& InPath) const
{
    return Utility::WriteFile(InPath, ToStr());
}

bool PropertyOwnerBase::Load(const String& InPath)
{
    CHECK_RETURN_LOG(!Utility::FileExists(InPath), "File does not exist: " + InPath, false)
    const String fileContent = Utility::ReadFile(InPath);
    CHECK_RETURN_LOG(fileContent.empty(), "Property file empty: " + InPath, false);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format: " + InPath, false);
    // Requires const, dont know why
    const rapidjson::Document& constDoc = doc;
    Deserialize(constDoc.GetObj());
    return true;   
}

bool PropertyOwnerBase::operator==(const PropertyOwnerBase& InOther) const
{
    // Compare properties!
    const auto& map = InOther.GetPropertyMap();
    for (const auto& entry : map)
    {
        const auto firstPtr = OffToPtr(entry.second);
        const auto secondPtr = InOther.OffToPtr(entry.second);
        if (firstPtr && secondPtr)
            if (!((*firstPtr) == (*secondPtr)))
                return false; 
    }
    return true; 
}

bool PropertyOwnerBase::AddPropertyInternal(PropertyBase* InProperty) const
{
    uint16 off = PtrToOff(InProperty);
    CHECK_RETURN(off == static_cast<uint16>(-1), false)
    String name = InProperty->GetName();
    GetPropertyMap()[name] = off;
    GetPropertyOrder().push_back(name);
    return true;
}

uint16 PropertyOwnerBase::PtrToOff(PropertyBase* InPtr) const
{
    const uint64 propertyAddr = reinterpret_cast<uint64>(InPtr);
    const uint64 thisAddr = reinterpret_cast<uint64>(this);
    const uint64 diffStart = propertyAddr - thisAddr;
    CHECK_RETURN(diffStart > Size(), static_cast<uint16>(-1));
    return static_cast<uint16>(diffStart);
}

PropertyBase* PropertyOwnerBase::OffToPtr(const uint16 InOff) const
{
    const uint64 propertyAddr = InOff;
    const uint64 thisAddr = reinterpret_cast<uint64>(this);
    const uint64 result = thisAddr + propertyAddr;
    
    CHECK_ASSERT(propertyAddr > Size(), "Address difference too great, property belongs to another class");
    
    return reinterpret_cast<PropertyBase*>(result);
}
