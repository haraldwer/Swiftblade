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
        static void OnList(const Nakama::NRpc& InRpc);
        static void OnListFailed(const Nakama::NError& InError);
        static void OnWrite(const Nakama::NRpc& InRpc);
        static void OnWriteFailed(const Nakama::NError& InError);
    };
}
