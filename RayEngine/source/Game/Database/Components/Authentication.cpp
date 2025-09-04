#include "Authentication.h"

#include <nakama-cpp/NClientInterface.h>

#include "Database/Manager.h"

void DB::Authentication::Init(Manager *InManager)
{
    Component::Init(InManager);

    onAuthenticated.Bind([&](auto InResp)
    {
        CHECK_RETURN(!InResp.success);
        LOG("Authenticated");
        authenticated = true;

        //RtClient& rtClient = GetRealtimeClient();
        //CHECK_RETURN_LOG(!rtClient, "Invalid realtime client");
        //rtClient->connect(GetSession(), true);
    });

    onSteam.Bind([&](auto InResp)
    {
        if (!InResp.success)
        {
            LOG("Failed to authenticate steam: " + InResp.error);
            AuthenticateDevice(); // Try to authenticate device instead
            return;
        }
        Event<AuthResponse>().Invoke({ InResp.success, InResp.error });
    });

    onDevice.Bind([&](auto InResp)
    {
        if (!InResp.success)
        {
            LOG("Failed to authenticate device: " + InResp.error);
        }
        Event<AuthResponse>().Invoke({ InResp.success, InResp.error });
    });
}

void DB::Authentication::Deinit()
{
    Component::Deinit();
    authenticated = false;
    onAuthenticated = {};
    onDevice = {};
    onSteam = {};
}

void DB::Authentication::Authenticate()
{
    // TODO: Later switch to steam
    AuthenticateDevice();
}

void DB::Authentication::AuthenticateDevice()
{
    const Client& client = GetClient();
    
    if (!client)
    {
        Event<DeviceAuthResponse>().Invoke({ false , "Invalid client" });
        return;
    }

    auto fail = [](const Nakama::NError& InError)
    {
        Event<DeviceAuthResponse>().Invoke({ false , toString(InError) });
    };

    auto success = [](const Nakama::NSessionPtr& InSession)
    {
        auto& man = Manager::Get();
        man.session = InSession;

        if (!man.session)
            Event<DeviceAuthResponse>().Invoke({ false, "Invalid session"});
        else
            Event<DeviceAuthResponse>().Invoke({ true, ""});
    };

    // Typically you would get the system's unique device identifier here.
    String id = Utility::DeviceGUID();
    const Nakama::NStringMap vars;
    
    // Authenticate with the Nakama server using Device Authentication.
    client->authenticateDevice(
        id,
        "",
        true,
        vars,
        success,
        fail);
}

void DB::Authentication::AuthenticateSteam()
{
    const Client& client = GetClient();
    
    if (!client)
    {
        Event<SteamAuthResponse>().Invoke({ false , "Invalid client" });
        return;
    }

    auto fail = [](const Nakama::NError& InError)
    {
        Event<SteamAuthResponse>().Invoke({ false , toString(InError) });
    };

    auto success = [](const Nakama::NSessionPtr& InSession)
    {
        auto& man = Manager::Get();
        man.session = InSession;

        if (!man.session)
            Event<SteamAuthResponse>().Invoke({ false, "Invalid session"});
        else
            Event<SteamAuthResponse>().Invoke({ true, ""});
    };

    String token = "..."; // Use steam token!
    bool create = true;
    const Nakama::NStringMap vars;
    client->authenticateSteam(token, "", create, vars, success, fail);
}

void DB::Authentication::LinkSteam() const
{
    CHECK_RETURN_LOG(!authenticated, "Cannot link, not authenticated");
    const Client& client = GetClient();
    const Session& session = GetSession();
    
    auto fail = [](const Nakama::NError& InError)
    {
        Event<SteamLinkResponse>().Invoke({ false , toString(InError) });
    };

    auto success = []
    {
        Event<SteamLinkResponse>().Invoke({ true });
    };

    const String token = "...";
    client->linkSteam(session, token, success, fail);
}

bool DB::Authentication::IsAuthenticated() const
{
    return GetSession().get() != nullptr && authenticated;
}
