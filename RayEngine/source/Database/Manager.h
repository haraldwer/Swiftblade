#pragma once

#include "Utility/Singelton.h"

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
        
    private:
        std::shared_ptr<Nakama::NClientInterface> Client = nullptr; 
        std::shared_ptr<Nakama::NRtClientInterface> RtClient = nullptr; 
    };
}
