#include "Manager.h"
#include "nakama-cpp/Nakama.h"

void DB::Manager::Init()
{
    Nakama::NClientParameters params;
    params.serverKey = "defaultkey"; // TODO: 
    params.host = "127.0.0.1"; // TODO: 
    params.port = Nakama::DEFAULT_PORT;
    Client = createDefaultClient(params);
    CHECK_ASSERT(!Client, "Failed to create client");
    RtClient = Client->createRtClient();
    CHECK_ASSERT(!Client, "Failed to create realtime client");
    
    Auth.Init(this);
    
    OnLoggedIn.Bind([](const OnLoginSuccess& OnSuccess)
    {
        // Delay initialization until authenticated
        auto& man = Get();
        man.Blob.Init(&man); 
        man.LB.Init(&man);
    });

    LOG("DB initialized");
}

void DB::Manager::Deinit()
{
    if (RtClient)
    {
        if (RtClient->isConnected())
            RtClient->disconnect();
        RtClient = nullptr;
    }

    if (Client)
    {
        Client->disconnect();
        Client = nullptr; 
    }

    LB.Deinit();
    Blob.Deinit();
    Auth.Deinit();
}

void DB::Manager::Update() const
{
    Client->tick();
    if (RtClient)
        RtClient->tick();
}
