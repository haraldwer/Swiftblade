#include "Manager.h"

#ifdef NAKAMA_ENABLE

#include "nakama-cpp/Nakama.h"

void DB::Manager::Init()
{
    config.LoadConfig();
    
    Nakama::NClientParameters params;
    params.serverKey = config.ServerKey;
    params.host = config.Address; 
    params.port = config.Port;
    params.ssl = config.SSL;

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

void DB::Manager::Tick() const
{
    client->tick();
    if (rtClient)
        rtClient->tick();
}


#else

void DB::Manager::Init() {}
void DB::Manager::Update() const {}
void DB::Manager::Deinit() {}

#endif
