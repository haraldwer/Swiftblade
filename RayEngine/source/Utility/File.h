#pragma once
#include <fstream>

namespace Utility
{
    inline String ReadFile(const String& InPath)
    {
        constexpr std::size_t read_size = 4096;
        auto stream = std::ifstream(("../content/" + InPath).data());
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

    inline bool WriteFile(const String& InPath, const String& InContent)
    {
        std::ofstream out("../content/" + InPath);
        if (!out.is_open())
            return false;
        out.clear();
        out << InContent;
        out.close();
        return true;
    }
}
