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
            PROPERTY_D(int, Plays, 0);
            PROPERTY_D(bool, Fav, false);
        };

        PROPERTY(String, List);
        PROPERTY(Vector<Entry>, Entries);
        
        static String RPC() { return "rpc_level_list"; }
    };

    struct RPCRoomList : PropertyOwner<RPCRoomList>
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
        };

        PROPERTY(String, List);
        PROPERTY(Vector<Entry>, Entries);
        
        static String RPC() { return "rpc_room_list"; }
    };
}
