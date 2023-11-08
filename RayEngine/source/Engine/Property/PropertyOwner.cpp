#include "PropertyOwner.h"

#include "PropertyBase.h"
#include "Utility/File.h"
#include "Utility/JsonUtility.h"

void PropertyOwnerBase::AddProperty(PropertyBase* InProperty)
{
    CHECK_RETURN(!Instance); 
    CHECK_ASSERT(!InProperty, "Property null");
    Instance->AddPropertyInternal(InProperty);
}

void PropertyOwnerBase::Serialize(SerializeObj& InOutObj) const
{
    for (const auto& p : GetPropertyMap())
    {
        const PropertyBase* ptr = OffToPtr(p.second);
        CHECK_ASSERT(!ptr, "Invalid property");
        ptr->Serialize(InOutObj);
    }
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
    return success;
}

bool PropertyOwnerBase::Edit()
{
    bool edited = false;
    for (const auto& p : GetPropertyMap())
    {
        PropertyBase* ptr = OffToPtr(p.second);
        CHECK_ASSERT(!ptr, "Invalid property");
        if (ptr->Edit())
            edited = true;
    }
    return edited;
}

bool PropertyOwnerBase::Save(const String& InPath) const
{
    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    writer.StartObject();
    Serialize(writer); 
    writer.EndObject();
    const String result = Utility::FormatJson(s.GetString());
    return Utility::WriteFile(InPath, result);
}

bool PropertyOwnerBase::Load(const String& InPath)
{
    const String fileContent = Utility::ReadFile(InPath);
    CHECK_RETURN_LOG(fileContent.empty(), "Prefab file empty", false);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format", false);
    Deserialize(doc.GetObj());
    return true; 
}

void PropertyOwnerBase::AddPropertyInternal(PropertyBase* InProperty) const
{
    GetPropertyMap()[InProperty->GetName()] = PtrToOff(InProperty);
}

uint16 PropertyOwnerBase::PtrToOff(PropertyBase* InPtr) const
{
    const uint64 propertyAddr = reinterpret_cast<uint64>(InPtr);
    const uint64 thisAddr = reinterpret_cast<uint64>(this);
    const uint64 diff = propertyAddr - thisAddr;
    CHECK_ASSERT(diff > 1000, "Address difference too great, something is wrong");
    return static_cast<uint16>(diff);
}

PropertyBase* PropertyOwnerBase::OffToPtr(const uint16 InOff) const
{
    const uint64 propertyAddr = static_cast<uint64>(InOff);
    const uint64 thisAddr = reinterpret_cast<uint64>(this);
    const uint64 result = thisAddr + propertyAddr;
    return reinterpret_cast<PropertyBase*>(result);
}
