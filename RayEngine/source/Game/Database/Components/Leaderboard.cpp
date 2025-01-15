#include "Leaderboard.h"

#include "Database/Manager.h"
#include "Database/Data/LBData.h"

void DB::Leaderboard::List(const LBRequest& InRequest)
{
    Client client = GetClient();
    Session session = GetSession();
    if (!client || !session)
    {
        LOG("Not connected");
        DBEvent<OnListError> listFailed;
        listFailed.Invoke({"", "Not connected"});
        return;
    }

    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    InRequest.Serialize(writer);
    const String json = Utility::FormatJson(s.GetString());

    auto success = [&](const Nakama::NRpc& InRpc) { OnList(InRpc); };
    auto error = [&](const Nakama::NError& InError) { OnListFailed(InError); };

    client->rpc(
        session,
        "Lua_LB_List",
        json,
        success,
        error);

    LOG("Listing leaderboard: " + json);
}

void DB::Leaderboard::OnList(const Nakama::NRpc& InRpc)
{
    LOG("Leaderboard received: " + InRpc.payload);
    rapidjson::Document doc;
    doc.Parse(InRpc.payload.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format: " + InRpc.payload);
    
    const rapidjson::Document& constDoc = doc;
    LBData data;
    CHECK_RETURN_LOG(data.Deserialize(constDoc), "Failed to parse json: " + InRpc.payload);
    
    DBEvent<OnListSuccess> success;
    success.Invoke({ data });
}

void DB::Leaderboard::OnListFailed(const Nakama::NError& InError)
{
    LOG("An error occurred: " + InError.message);
    DBEvent<OnListError> error;
    error.Invoke({ DB::GetErrorString(InError), InError.message});
}

void DB::Leaderboard::Write(const LBSubmission& InSubmission)
{
    const auto client = GetClient();
    const auto session = GetSession();
    if (!client || !session)
    {
        LOG("DB not yet connected");
        DBEvent<OnListError> listFailed;
        listFailed.Invoke({"", "GameDB not yet connected on lb list"});
        return;
    }

    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    InSubmission.Serialize(writer);
    const String json = Utility::FormatJson(s.GetString());

    auto success = [&](const Nakama::NRpc& InRpc) { OnWrite(InRpc); };
    auto error = [&](const Nakama::NError& InError) { OnWriteFailed(InError); };

    client->rpc(
        session,
        "Lua_LB_Write",
        json,
        success,
        error);
    
    LOG("Submitting score to leaderboard: " + json);
}

void DB::Leaderboard::OnWrite(const Nakama::NRpc& InRpc)
{
    LOG("Score submitted: " + InRpc.payload);
    DBEvent<OnWriteSuccess> success;
    success.Invoke({ InRpc.payload });
}

void DB::Leaderboard::OnWriteFailed(const Nakama::NError& InError)
{
    LOG("An error occurred: " + InError.message);
    DBEvent<OnWriteError> error;
    error.Invoke({ DB::GetErrorString(InError), InError.message});
}

