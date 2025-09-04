#pragma once

namespace DB
{
    struct RPCLevelList : PropertyOwner<RPCLevelList>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, List);
        };
		
		
        static String RPC() { return "rpc_level_list"; }
    };
}
