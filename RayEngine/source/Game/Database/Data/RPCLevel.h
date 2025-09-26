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
        PROPERTY(Level, Data);
        
        static String RPC() { return "rpc_level_info"; }
    };

    struct RPCLevelList : PropertyOwner<RPCLevelList>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, List);
        };

        struct Entry : PropertyOwner<Entry>
        {
            PROPERTY(String, ID);
            PROPERTY(String, Name);
            PROPERTY(String, Creator);
            PROPERTY_D(int, Plays, 0);
            PROPERTY_D(bool, Fav, false);
        };

        PROPERTY(String, List);
        PROPERTY(Vector<Entry>, Entries);
        
        static String RPC() { return "rpc_level_list"; }
    };

    struct RPCSubmitLevel : PropertyOwner<RPCSubmitLevel>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, Hash);
            PROPERTY(Level, Data);
        };

        String ID;

        static String RPC() { return "rpc_submit_level"; }
    };
}

