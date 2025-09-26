#pragma once
#include "Level/Room.h"
#include "Property/PropertyOwner.h"

namespace DB
{
    struct RPCRoomInfo : PropertyOwner<RPCRoomInfo>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, ID);
        };

        PROPERTY(String, ID);
        PROPERTY(Room, Data);
        
        static String RPC() { return "rpc_level_info"; }
    };

    struct RPCRoomList : PropertyOwner<RPCRoomList>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, List);
        };

        PROPERTY(String, List);
        PROPERTY(Vector<RoomEntry>, Entries);
        
        static String RPC() { return "rpc_room_list"; }
    };

    struct RPCSubmitRoom : PropertyOwner<RPCSubmitRoom>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(Room, Data);
        };

        PROPERTY(String, RoomID);
        PROPERTY(String, Error);
        PROPERTY(bool, Success);

        static String RPC() { return "rpc_submit_room"; }
    };

}
