#include "RPC.h"

#ifdef NAKAMA_ENABLE

#include "Database/Manager.h"
#include "nakama-cpp/NClientInterface.h"

void DB::RPC::CallRPC(const String &InRPC, const String& InData, const std::function<void(const Nakama::NRpc&)>& InSuccess, const std::function<void(const Nakama::NError&)> &InError) const
{
    if (!GetDB().auth.IsAuthenticated())
    {
        InError(Nakama::NError("Not connected to DB", Nakama::ErrorCode::Unknown));
        return;
    }
    
    const Client& client = GetClient();
    if (!client)
    {
        InError(Nakama::NError("Invalid client", Nakama::ErrorCode::Unknown));
        return;
    }

    const Session& session = GetSession();
    if (!session)
    {
        InError(Nakama::NError("Invalid session", Nakama::ErrorCode::Unknown));
        return;
    }
    
    client->rpc(
        session,
        InRPC,
        InData,
        InSuccess,
        InError);
    
    LOG("RPC: " + InRPC + " | " + InData);
}

String DB::RPC::GetError(const Nakama::NError &InError)
{
    return toString(InError);
}

String DB::RPC::GetPayload(const Nakama::NRpc &InRpc)
{
    return InRpc.payload;
}

#else

void DB::RPC::CallRPC(const String &InRPC, const String &InData,
    const std::function<void(const Nakama::NRpc &)> &InSuccess,
    const std::function<void(const Nakama::NError &)> &InError) const {}

String DB::RPC::GetError(const Nakama::NError &InError) {}
String DB::RPC::GetPayload(const Nakama::NRpc &InRpc) {}

#endif