#pragma once
#include <fstream>

namespace Utility
{
    inline String ReadFile(const String& path)
    {
        constexpr std::size_t read_size = 4096;
        auto stream = std::ifstream(path.data());
        stream.exceptions(std::ios_base::badbit);

        if (!stream) {
            return String();
        }
    
        auto out = std::string();
        auto buf = std::string(read_size, '\0');
        while (stream.read(buf.data(), read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        out.append(buf, 0, stream.gcount());
        return out;
    }
}
