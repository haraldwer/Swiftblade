#pragma once
#include "Level/RoomInfo.h"
#include "Property/PropertyOwner.h"

namespace DB
{
    struct RPCRoomInfo : PropertyOwner<RPCRoomInfo>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, ID);
        };

        PROPERTY(RoomData, Data);
        PROPERTY(RoomInfo, Info);
        PROPERTY(String, Scene)
        
        static String RPC() { return "rpc_room_info"; }
    };

    struct RPCRoomList : PropertyOwner<RPCRoomList>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, List);
            PROPERTY(Vector<String>, Entries);
        };

        PROPERTY(String, List);
        PROPERTY(Vector<RoomInfo>, Entries);
        
        static String RPC() { return "rpc_room_list"; }
    };

    struct RPCSubmitRoom : PropertyOwner<RPCSubmitRoom>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(RoomInfo, Info);
            PROPERTY(String, Scene);
        };

        PROPERTY(String, ID);
        PROPERTY(String, Error);
        PROPERTY(bool, Success);

        static String RPC() { return "rpc_submit_room"; }
    };

}
