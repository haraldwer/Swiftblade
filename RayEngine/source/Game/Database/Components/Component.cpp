#include "Component.h"

#include "Database/Manager.h"

void DB::Component::Init(DB::Manager* InManager)
{
    Manager = InManager; 
}

void DB::Component::Deinit()
{
    Manager = nullptr;
}

DB::Manager& DB::Component::GetDB() const
{
    CHECK_ASSERT(!Manager, "Invalid DB manager");
    return *Manager; 
}

DB::Client& DB::Component::GetClient() const
{
    return GetDB().Client;
}

DB::RtClient& DB::Component::GetRealtimeClient() const
{
    return GetDB().RtClient;
}

DB::Session& DB::Component::GetSession() const
{
    return GetDB().Session;
}
