#pragma once

namespace Nakama
{
    class NSessionInterface;
    class NClientInterface;
    class NRtClientInterface;
    struct NError;
    struct NRpc;
    using NStorageObjects = std::vector<struct NStorageObject>;
    using NStorageObjectAcks = std::vector<struct NStorageObjectAck>;
}

namespace DB
{
    using Client = std::shared_ptr<Nakama::NClientInterface>; 
    using RtClient = std::shared_ptr<Nakama::NRtClientInterface>; 
    using Session = std::shared_ptr<Nakama::NSessionInterface>;
}