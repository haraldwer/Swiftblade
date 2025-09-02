#pragma once

namespace DB
{
    struct RPCSubmitRoom : PropertyOwner<RPCSubmitRoom>
    {
        struct Request : PropertyOwner<Request>
        {
            String name;
            String date;
            String data;
            int size;
            int enemies;
        };
        
    };

    struct RPCSubmitLevel : PropertyOwner<RPCSubmitLevel>
    {
        struct Request : PropertyOwner<Request>
        {
            String name;
            String hash;
            String date;
            String rooms;
        };
    
    };
}
