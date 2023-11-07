#include "PropertyOwner.h"

#include "PropertyBase.h"
#include "Utility/File.h"
#include "Utility/JsonUtility.h"

void PropertyOwner::StartScope()
{
    Instance = this;  
}

void PropertyOwner::EndScope()
{
    Instance = nullptr;
}

void PropertyOwner::AddProperty(PropertyBase* InProperty)
{
    CHECK_ASSERT(!InProperty, "Property null");
    CHECK_ASSERT(!Instance, "No instance");
    Instance->Properties[InProperty->GetName()] = InProperty;
}

void PropertyOwner::Serialize(SerializeObj& InOutObj) const
{
    for (auto& p : Properties)
        p.second->Serialize(InOutObj);
}

bool PropertyOwner::Deserialize(const DeserializeObj& InObj)
{
    bool success = true;
    for (auto& p : Properties)
        if (!p.second->Deserialize(InObj))
            success = false;
    return success;
}

bool PropertyOwner::Edit()
{
    bool edited = false;
    for (auto& p : Properties)
        if (p.second->Edit())
            edited = true;
    return edited;
}

bool PropertyOwner::Save(const String& InPath) const
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

bool PropertyOwner::Load(const String& InPath)
{
    const String fileContent = Utility::ReadFile(InPath);
    CHECK_RETURN_LOG(fileContent.empty(), "Prefab file empty", false);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format", false);
    Deserialize(doc.GetObj());
    return true; 
}
