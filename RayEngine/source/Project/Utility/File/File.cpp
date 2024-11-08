#include "File.h"

#include <filesystem>
#include <fstream>

void Utility::SetWorkingDir()
{
    auto curr = std::filesystem::current_path();
    while (!curr.string().ends_with("RayEngine"))
    {
        String file = curr.filename().string();
        curr = curr.parent_path();
    }
    LOG("Setting working directory to \"" + curr.string() + "\"");
    std::filesystem::current_path(curr.concat("\\content")); 
}

String Utility::ReadFile(const String& InPath)
{
    constexpr std::size_t read_size = 4096;
    auto stream = std::ifstream(InPath.c_str());
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

    // Remove weird characters at start
    size_t off = 0;
    while (off < out.length() && out[off] < 0)
        off++;
    
    return out.substr(off);
}

bool Utility::WriteFile(const String& InPath, const String& InContent)
{
    std::ofstream out(InPath);
    if (!out.is_open())
        return false;
    out.clear();
    out << InContent;
    out.close();
    return true;
}

bool Utility::FileExists(const String& InPath)
{
    return std::filesystem::exists(InPath);
}

Utility::Timepoint Utility::GetFileWriteTime(const String& InPath)
{
    if (InPath.empty())
        return Timepoint();
    const String path = InPath;
    if (!std::filesystem::exists(path))
        return Timepoint(); 
    const std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(path);
    return Timepoint(fileTime.time_since_epoch());
}
