#include "Manager.h"
#include "nakama-cpp/Nakama.h"

void DB::Manager::Init()
{
    // Create client
    Nakama::NClientParameters params;
    params.serverKey = "defaultkey";
    params.host = "127.0.0.1";
    params.port = Nakama::DEFAULT_PORT;
    Client = createDefaultClient(params);
    
    RtClient = Client->createRtClient();
    
    // Typically you would get the system's unique device identifier here.
    const String deviceId = "e872f976-34c1-4c41-88fe-fd6aef118782";
    
    auto loginFailedCallback = [](const Nakama::NError& error)
    {
        LOG("An error occurred: " + error.message);
    };

    auto loginSucceededCallback = [&](const Nakama::NSessionPtr& session)
    {
        LOG("Successfully authenticated: " + session->getAuthToken());
        RtClient->connect(session, true);

    };

    // Authenticate with the Nakama server using Device Authentication.
    Client->authenticateDevice(
        deviceId,
        nonstd::nullopt,
        nonstd::nullopt,
        {},
        loginSucceededCallback,
        loginFailedCallback);
}

void DB::Manager::Update()
{
    Client->tick();
    
    if (RtClient)
    {
        RtClient->tick();
    }

}
