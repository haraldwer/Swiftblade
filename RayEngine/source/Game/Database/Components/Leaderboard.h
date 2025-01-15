#pragma once
#include "Component.h"

namespace DB
{
    struct LBSubmission;
    struct LBRequest;

    class Leaderboard : public Component
    {
    public:
        void List(const LBRequest& InRequest);
        void Write(const LBSubmission& InSubmission);
    private:
        void OnList(const Nakama::NRpc& InRpc);
        void OnListFailed(const Nakama::NError& InError);
        void OnWrite(const Nakama::NRpc& InRpc);
        void OnWriteFailed(const Nakama::NError& InError);
    };
}
