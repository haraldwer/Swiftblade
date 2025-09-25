#pragma once
#include "Level/Level.h"

namespace DB
{
    struct RPCLevelInfo : PropertyOwner<RPCLevelInfo>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, ID);
        };

        PROPERTY(String, ID);
        PROPERTY(::Level, Level);
        
        static String RPC() { return "rpc_level_info"; }
    };

    struct RPCRoomInfo : PropertyOwner<RPCRoomInfo>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, ID);
        };

        PROPERTY(String, ID);
        PROPERTY(String, Scene);
        // Also other room info
        
        static String RPC() { return "rpc_level_info"; }
    };
}

