#include "Manager.h"
#include "nakama-cpp/Nakama.h"

void DB::Manager::Init()
{
    Nakama::NClientParameters params;
    params.serverKey = "defaultkey"; // TODO: 
    params.host = "127.0.0.1"; // TODO: 
    params.port = Nakama::DEFAULT_PORT;
    client = createDefaultClient(params);
    CHECK_ASSERT(!client, "Failed to create client");
    rtClient = client->createRtClient();
    CHECK_ASSERT(!client, "Failed to create realtime client");
    
    auth.Init(this);
    rpc.Init(this);
    user.Init(this);
    LOG("DB initialized");
}

void DB::Manager::Deinit()
{
    if (rtClient)
    {
        if (rtClient->isConnected())
            rtClient->disconnect();
        rtClient = nullptr;
    }

    if (client)
    {
        client->disconnect();
        client = nullptr; 
    }

    rpc.Deinit();
    auth.Deinit();
}

void DB::Manager::Update() const
{
    client->tick();
    if (rtClient)
        rtClient->tick();
}
