#include "Component.h"

#include "Database/Manager.h"


void DB::Component::Init(Manager* InManager)
{
    manager = InManager; 
}

void DB::Component::Deinit()
{
    manager = nullptr;
}

DB::Manager& DB::Component::GetDB() const
{
    CHECK_ASSERT(!manager, "Invalid DB manager");
    return *manager; 
}

DB::Client& DB::Component::GetClient() const
{
    return GetDB().client;
}

DB::RtClient& DB::Component::GetRealtimeClient() const
{
    return GetDB().rtClient;
}

DB::Session& DB::Component::GetSession() const
{
    return GetDB().session;
}
