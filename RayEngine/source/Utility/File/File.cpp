#include "File.h"

#include <filesystem>
#include <fstream>
#include <ranges>

void Utility::SetWorkingDir()
{
#ifndef __EMSCRIPTEN__
    std::filesystem::path curr = std::filesystem::current_path();
    while (!curr.string().ends_with("RayEngine") && !curr.empty())
    {
        String file = curr.filename().string();
        curr = curr.parent_path();
    }
    const std::filesystem::path newDir = curr.concat("/content");
    LOG("Setting working directory to \"" + newDir.string() + "\"");
    std::filesystem::current_path(newDir);
#endif
}

String Utility::ReadFile(const String& InPath)
{
    constexpr std::size_t readSize = 4096;
    auto stream = std::ifstream(InPath.c_str());
    stream.exceptions(std::ios_base::badbit);

    if (!stream) {
        return String();
    }
    
    auto out = std::string();
    auto buf = std::string(readSize, '\0');
    while (stream.read(buf.data(), readSize))
        out.append(buf, 0, stream.gcount());
    out.append(buf, 0, stream.gcount());

    // Remove weird characters at start
    size_t off = 0;
    while (off < out.length() && out[off] < 0)
        off++;
    
    return out.substr(off);
}

Vector<String> Utility::ListFiles(const String &InPath)
{
    if (!std::filesystem::exists(InPath))
        return {};
    
    Vector<String> result;
    for (auto& e : std::filesystem::directory_iterator(InPath))
    {
        if (!e.is_directory())
        {
            String path = e.path().relative_path();
            LOG("Path found: " + path);
            result.push_back(path);
        }
    }
    return result;
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

void Utility::CreateDir(const String &InPath)
{
    auto path = std::filesystem::path(InPath);
    if (path.has_extension())
        path = path.parent_path();
    Vector<std::filesystem::path> stack;
    while (!std::filesystem::exists(path) && !path.empty())
    {
        stack.push_back(path);
        path = path.parent_path();
    }
    for (auto p : std::ranges::views::reverse(stack))
    {
        LOG("Creating: " + p.string());
        std::filesystem::create_directory(p);
    }
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

String Utility::GetCachePath(const String &InPath, String InExt)
{
    if (!InExt.contains("."))
        InExt = "." + InExt;
    return "Cache/" + ToStr(Hash(InPath)) + InExt; 
}
