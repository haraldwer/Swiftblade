#pragma once

namespace DB
{
    struct RPCSubmitRoom : PropertyOwner<RPCSubmitRoom>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, Name);
            PROPERTY(String, Scene);
            PROPERTY(Vec3I, Size);
            PROPERTY(float, Length);
            PROPERTY(int, Spawners);
            PROPERTY(int, Objects);
        };

        PROPERTY(String, RoomID);
        PROPERTY(String, Error);
        PROPERTY(bool, Success)

        static String RPC() { return "rpc_submit_room"; }
    };

    struct RPCSubmitLevel : PropertyOwner<RPCSubmitLevel>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, name);
            PROPERTY(String, hash);
            PROPERTY(String, date);
            PROPERTY(String, rooms);
        };
    
    };
}
