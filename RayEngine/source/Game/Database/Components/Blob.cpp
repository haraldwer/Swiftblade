#include "Blob.h"

#include <nakama-cpp/NClientInterface.h>

#include "Database/Manager.h"
#include "Database/NakamaError.h"

void DB::Blob::Init(DB::Manager* InManager)
{
    Component::Init(InManager);
    const Client& client = GetClient();
    const Session& session = GetSession();
    if (!client || !session)
    {
        LOG("Not connected")
        DBEvent<OnBlobReadError> error;
        error.Invoke({ "", "Not connected" });
    }

    auto success = [&](const Nakama::NStorageObjects& InObjects)
    {
        OnReadSuccess(InObjects);
    };

    auto error = [&](const Nakama::NError& InError)
    {
        OnReadFailed(InError);
    };

    Vector<Nakama::NReadStorageObjectId> objects;
    for (auto& p : data.GetProperties())
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
    
    const auto properties = data.GetProperties();
    for (auto& object : InObjects)
    {
        String propertyName = object.key;
        String json = object.value;
        CHECK_CONTINUE_LOG(!properties.contains(propertyName), "Property " + propertyName + " does not exist");
        const auto ptr = properties.at(propertyName);
        CHECK_CONTINUE_LOG(!properties.contains(propertyName), "Invalid ptr for " + propertyName);
        DocumentObj doc;
        doc.Parse(json.c_str());
        CHECK_CONTINUE_LOG(!doc.IsObject(), "Failed to parse: " + json);
        const rapidjson::Document& constDoc = doc;
        ptr->Deserialize(constDoc.GetObj());
    }
	
    data.Initialized = true;
    DBEvent<OnBlobReadSuccess> success;
    success.Invoke({});
}

void DB::Blob::OnReadFailed(const Nakama::NError& InError)
{
    LOG("An error occurred: " + InError.message);
    DBEvent<OnBlobReadError> error;
    error.Invoke({ GetErrorString(InError), InError.message });
}

void DB::Blob::Set(const BlobData& InData)
{
    if (!data.Initialized)
    {
        data = InData;
        LOG("Blob not yet initialized");
        DBEvent<OnBlobWriteError> error;
        error.Invoke({ "", "Not yet initialized"});
        return;
    }
	
    const auto client = GetClient();
    const auto session = GetSession();
    if (!client || !session)
    {
        LOG("Not connected");
        DBEvent<OnBlobWriteError> error;
        error.Invoke({ "", "Not connected"});
        return;
    }

    Vector<Nakama::NStorageObjectWrite> objects;
    auto oldProperties = data.GetProperties();
    auto newProperties = InData.GetProperties();
    for (auto& newProperty : InData.GetProperties())
    {
        CHECK_CONTINUE(!newProperty.second);
        String variableName = newProperty.second->GetName();
        auto oldProperty = oldProperties.at(variableName);
        CHECK_CONTINUE(!oldProperty);

        // Compare
        if ((*newProperty.second) == (*oldProperty))
            continue;
        
        rapidjson::StringBuffer s;
        rapidjson::Writer writer(s);
        newProperty.second->Serialize(writer);
        const String json = Utility::FormatJson(s.GetString());
        CHECK_CONTINUE_LOG(json.empty(), "Failed to serialize variable: " + variableName);
			
        auto& obj = objects.emplace_back();
        obj.collection = "blob";
        obj.key = variableName; // property name
        obj.value = json; // property json
        obj.permissionRead = Nakama::NStoragePermissionRead::OWNER_READ;
        obj.permissionWrite = Nakama::NStoragePermissionWrite::OWNER_WRITE;
    }

    if (objects.empty())
    {
        LOG("Nothing changed");
        DBEvent<OnBlobWriteError> error;
        error.Invoke({ "", "Nothing changed"});
        return;
    }
    
    auto success = [&](const Nakama::NStorageObjectAcks& InObjectAcks) { OnWriteSuccess(InObjectAcks); };
    auto error = [&](const Nakama::NError& InError) { OnWriteFailed(InError); };
    client->writeStorageObjects(session, objects, success, error);
}

void DB::Blob::OnWriteFailed(const Nakama::NError& InError)
{
    LOG("An error occurred: " + InError.message);
    DBEvent<OnBlobWriteError> error;
    error.Invoke({ GetErrorString(InError), InError.message });
}

void DB::Blob::OnWriteSuccess(const Nakama::NStorageObjectAcks& InObjectAcks)
{
    DBEvent<OnBlobWriteSuccess> success;
    success.Invoke({});
}