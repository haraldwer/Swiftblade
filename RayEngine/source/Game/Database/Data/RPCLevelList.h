#pragma once

namespace DB
{
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
            PROPERTY(int, Plays);
            PROPERTY(bool, Fav);
        };

        PROPERTY(String, List);
        PROPERTY(Vector<Entry>, Entries);
        
        static String RPC() { return "rpc_level_list"; }
    };
}
