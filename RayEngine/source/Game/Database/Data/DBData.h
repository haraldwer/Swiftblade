#pragma once

namespace DB
{
    template <class T>
    class DBData : public PropertyOwner<T>
    {
        // Custom db stuff here
        
    };

    struct OnError
    {
        String code;
        String message;
    };
}