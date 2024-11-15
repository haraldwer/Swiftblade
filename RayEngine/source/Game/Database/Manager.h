#pragma once
#include "Components/Authentication.h"
#include "Components/Blob.h"
#include "Components/Leaderboard.h"

namespace Nakama
{
    class NClientInterface;
    class NRtClientInterface;
}

namespace DB
{
    class Manager : public Utility::Singelton<Manager>
    {
    public:
        void Init();
        void Update();
        void Deinit();

        Authentication Auth;
        Leaderboard LB;
        Blob Blob;
        
    private:
        std::shared_ptr<Nakama::NClientInterface> Client = nullptr; 
        std::shared_ptr<Nakama::NRtClientInterface> RtClient = nullptr;

        
    };
}
