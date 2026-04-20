#include "User.h"

#include "Database/Manager.h"

#ifdef NAKAMA_ENABLE

void DB::User::Init(Manager *InManager)
{
    Component::Init(InManager);
    
    onLogin.Bind([&](auto& InResp, auto InC)
    {
        CHECK_RETURN_LOG(!InResp.success, InResp.error);
        if (InResp.data.LoggedIn)
        {
            username = InResp.data.Name;
            loggedIn = true;
            LOG("Logged in as " + username);
        }
    });
}

void DB::User::Deinit()
{
    Component::Deinit();
    onLogin = {};
    loggedIn = false;
}

void DB::User::TryLogin(const String &InName) const
{
    RPCLogin::Request request;
    request.Name = InName;
    GetDB().rpc.Request<RPCLogin>(request);
    
}

bool DB::User::IsLoggedIn() const
{
    return GetSession() && loggedIn;
}

#else

void DB::User::Init(Manager *InManager)
{
    Component::Init(InManager);
    loggedIn = true;
}

void DB::User::Deinit()
{
    Component::Deinit();
}

void DB::User::TryLogin(const String &InName) const
{
    LOG("TryLogin: " + InName);
}

bool DB::User::IsLoggedIn() const
{
    LOG("IsLoggedIn");
    return true;
}

#endif
