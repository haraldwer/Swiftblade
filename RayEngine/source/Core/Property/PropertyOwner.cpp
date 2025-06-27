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
    const auto& map = GetPropertyMap();
    bool header = false;
    if (map.size() <= 1 || Utility::MaybeCollapse(InName, InOffset, header))
    {
        if (header)
            ImGui::Indent();
        const uint32 offset = static_cast<uint32>(reinterpret_cast<uint64>(this)) + InOffset;
        for (const auto& p : map)
        {
            PropertyBase* ptr = OffToPtr(p.second);
            CHECK_ASSERT(!ptr, "Invalid property");
            if (ptr->Edit(offset))
                edited = true;
        }
        if (header)
            ImGui::Unindent();
    }
    return edited;
}

bool PropertyOwnerBase::Save(const String& InPath) const
{
    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    Serialize(writer); 
    const String result = Utility::FormatJson(s.GetString());
    return Utility::WriteFile(InPath, result);
}

bool PropertyOwnerBase::Load(const String& InPath)
{
    const String fileContent = Utility::ReadFile(InPath);
    CHECK_RETURN_LOG(fileContent.empty(), "Property file empty", false);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format", false);
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
    GetPropertyMap()[InProperty->GetName()] = off;
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
