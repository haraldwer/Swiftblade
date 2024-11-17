#include "Blob.h"

#include <nakama-cpp/NClientInterface.h>

void DB::Blob::Init(DB::Manager* InManager)
{
    Component::Init(InManager);
    Client& client = GetClient();
    Session& session = GetSession();
    CHECK_RETURN_LOG(!client, "No client");
    CHECK_RETURN_LOG(!session, "No session");

    auto success = [&](const Nakama::NStorageObjects& InObjects)
    {
        OnReadSuccess(InObjects);
    };

    auto error = [&](const Nakama::NError& InError)
    {
        OnReadFailed(InError);
    };

    Vector<Nakama::NReadStorageObjectId> objects;
    for (auto& p : Data.GetProperties())
    {
        auto& obj = objects.emplace_back(); 
        obj.collection = "blob";
        obj.key = p.first; 
        obj.userId = session->getUserId();
    }
	
    client->readStorageObjects(
        session,
        objects,
        success,
        error);
}

void DB::Blob::OnReadSuccess(const Nakama::NStorageObjects& InObjects)
{
    LOG("Blob read successfully");

    auto getObj = [](const DocumentObj& InDoc) -> DeserializeObj
    {
        const DocumentObj& doc = InDoc;
        const auto obj = doc.GetObj();
        return obj; 
    };
    
    const auto properties = Data.GetProperties();
    for (auto& object : InObjects)
    {
        String propertyName = object.key;
        String json = object.value;
        CHECK_CONTINUE_LOG(!properties.contains(propertyName), "Property " + propertyName + " does not exist");
        auto ptr = properties.at(propertyName);
        CHECK_CONTINUE_LOG(!properties.contains(propertyName), "Invalid ptr for " + propertyName);
        DocumentObj doc;
        doc.Parse(json.c_str());
        CHECK_CONTINUE_LOG(!doc.IsObject(), "Failed to parse: " + json); 
        ptr->Deserialize(getObj(doc));
    }
	
    Data.Initialized = true; 
}

void DB::Blob::OnReadFailed(const Nakama::NError& InError)
{
    LOG("An error occurred: " + InError.message);
}

