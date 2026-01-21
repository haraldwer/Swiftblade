#pragma once

namespace DB
{
    struct RPCLogin : PropertyOwner<RPCLogin>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, Name);
        };
		
        PROPERTY(String, Name);
        PROPERTY_D(bool, LoggedIn, false);
		
        static String RPC() { return "rpc_login_user"; }
    };
}
