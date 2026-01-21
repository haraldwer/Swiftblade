#pragma once
#include <filesystem>

namespace Utility
{
    inline String ToStr(const std::filesystem::path& InValue)
    {
        return InValue.string();
    }
}
