#include "Authentication.h"

#include <nakama-cpp/NClientInterface.h>

#include "Database/Manager.h"
#include "Database/NakamaError.h"

void DB::Authentication::Authenticate(const AuthData& InData) const
{
    Client& client = GetClient();
    if (!client)
    {
        LOG("Invalid client");
        DBEvent<OnLoginError> loginFailed;
        loginFailed.Invoke({ "" , "Invalid client" });
    }

    auto fail = [&](const Nakama::NError& InError)
    {
        OnFailed(InError);
    };

    auto success = [&](const Nakama::NSessionPtr& InSession)
    {
        GetDB().Session = InSession;
        OnSuccess();
    };

    // Typically you would get the system's unique device identifier here.
    String id;
    switch (static_cast<AuthMethod>(InData.Method.Get()))
    {
    case AuthMethod::DEVICE:
        id = Utility::DeviceGUID();
        break;
    }
    const Nakama::NStringMap vars;
    
    // Authenticate with the Nakama server using Device Authentication.
    client->authenticateDevice(
        id,
        InData.User.Get(),
        InData.Create,
        vars,
        success,
        fail);
}

bool DB::Authentication::IsAuthenticated() const
{
    return GetSession().get() != nullptr;
}

void DB::Authentication::OnSuccess() const
{
    Session& session = GetSession();
    CHECK_RETURN_LOG(!session, "Invalid session");
    LOG("Successfully authenticated: " + session->getAuthToken());
    DBEvent<OnLoginSuccess> loginSuccess;
    loginSuccess.Invoke({});

    RtClient& rtClient = GetRealtimeClient();
    CHECK_RETURN_LOG(!rtClient, "Invalid realtime client");
    rtClient->connect(session, true);
}

void DB::Authentication::OnFailed(const Nakama::NError& InError) const
{
    LOG("An error occurred: " + InError.message);
    DBEvent<OnLoginError> loginFailed;
    loginFailed.Invoke({ GetErrorString(InError) , InError.message });
}

