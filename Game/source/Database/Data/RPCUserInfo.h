#pragma once

namespace DB
{
    struct RPCUserInfo : PropertyOwner<RPCUserInfo>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, User)
        };

        PROPERTY(String, Username);
        PROPERTY(int, Plays);
        PROPERTY(int, Kills);
        PROPERTY(String, FirstLogin);

        static String RPC() { return "rpc_get_user_info"; }
    };

    struct RPCUserLB : PropertyOwner<RPCUserLB>
    {
        enum class LBType : uint8
        {
            TOP_RANKS = 0,
            MOST_PLAYED,
            RECENT_LEVELS,
            RECENT_ROOMS,
        };
        
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, User)            
            PROPERTY(int, LB);
        };

        static String RPC() { return "rpc_get_user_lb"; }
    };
    
    struct RPCUserFavs : PropertyOwner<RPCUserFavs>
    {
        struct Request : PropertyOwner<Request>
        {
        };


        static String RPC() { return "rpc_get_user_favourites"; }
    };

    struct RPCUserSubmission : PropertyOwner<RPCUserSubmission>
    {
        struct Request : PropertyOwner<Request>
        {
        };

        static String RPC() { return "rpc_get_user_submissions"; }
    };
}
