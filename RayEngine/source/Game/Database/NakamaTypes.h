#pragma once

namespace Nakama
{
    class NSessionInterface;
    class NClientInterface;
    class NRtClientInterface;
    struct NError;
    using NStorageObjects = std::vector<struct NStorageObject>;
}

namespace DB
{
    using Client = std::shared_ptr<Nakama::NClientInterface>; 
    using RtClient = std::shared_ptr<Nakama::NRtClientInterface>; 
    using Session = std::shared_ptr<Nakama::NSessionInterface>;
}