#pragma once

namespace DB
{
    template <class T>
    struct Response
    {
        // Custom db stuff here
        bool success;
        String error;
        T data;
    };
}