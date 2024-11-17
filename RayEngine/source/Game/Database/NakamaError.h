#pragma once

#include <nakama-cpp/NError.h>

namespace DB
{
    inline String GetErrorString(const Nakama::NError& InError)
    {
        switch (InError.code)
        {
        case Nakama::ErrorCode::Unknown:
            return "Unknown";
        case Nakama::ErrorCode::NotFound:
            return "Not found";
        case Nakama::ErrorCode::AlreadyExists:
            return "Already exists";
        case Nakama::ErrorCode::InvalidArgument:
            return "Invalid arguments";
        case Nakama::ErrorCode::Unauthenticated:
            return "Unauthenticated";
        case Nakama::ErrorCode::PermissionDenied:
            return "PermissionDenied";
        case Nakama::ErrorCode::ConnectionError:
            return "Connection error";
        case Nakama::ErrorCode::InternalError:
            return "Internal error";
        case Nakama::ErrorCode::CancelledByUser:
            return "Cancelled by user";
        }
        return "Unknown";
    }
}
