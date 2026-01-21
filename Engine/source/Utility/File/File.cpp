#include "File.h"

#include <filesystem>
#include <fstream>
#include <ranges>

void Utility::File::SetWorkingDir()
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

String Utility::File::Read(const String& InPath)
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

Vector<String> Utility::File::List(const String &InPath)
{
    CHECK_RETURN(!std::filesystem::exists(InPath), {})
    Vector<String> result;
    for (auto& e : std::filesystem::directory_iterator(InPath))
        if (!e.is_directory())
            result.push_back(e.path().relative_path().string());
    return result;
}

bool Utility::File::Write(const String& InPath, const String& InContent)
{
    std::ofstream out(InPath);
    if (!out.is_open())
        return false;
    out.clear();
    out << InContent;
    out.close();
    return true;
}

bool Utility::File::Exists(const String& InPath)
{
    return std::filesystem::exists(InPath);
}

void Utility::File::CreateDir(const String &InPath)
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

bool Utility::File::Delete(const String &InPath)
{
    LOG("Deleting " + InPath)
    CHECK_RETURN(!Exists(InPath), false)
    return std::filesystem::remove(InPath);
}

bool Utility::File::Copy(const String &InFrom, const String &InTo, bool InRename)
{
    if (!Exists(InFrom))
    {
        LOG("Copy source " + InFrom + " does not exist");
        return false;
    }
    
    String newPath = InTo;
    if (Exists(newPath) && InRename)
    {
        auto s = std::filesystem::path(InTo);
        String pathStart = s.parent_path().string();
        String fullName = Name(s.string());
        auto extLoc = fullName.find_last_of('.');
        String ext = extLoc == std::string::npos ? "" : fullName.substr(extLoc);
        String name = fullName.substr(0, extLoc);

        int postfix = 0;
        do
        {
            postfix++;
            newPath = pathStart + "/" + name + "_" + ToStr(postfix) + ext;
        }
        while (Exists(newPath));
    }
    
    LOG("Copying " + InFrom + " to " + newPath);
    std::filesystem::copy(InFrom, newPath, std::filesystem::copy_options::overwrite_existing);
    return Exists(newPath);
}

Utility::Timepoint Utility::File::GetWriteTime(const String& InPath)
{
    if (InPath.empty())
        return Timepoint();
    const String path = InPath;
    if (!std::filesystem::exists(path))
        return Timepoint(); 
    const std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(path);
    return Timepoint(fileTime.time_since_epoch());
}

uint32 Utility::File::GetSize(const String &InPath)
{
    return static_cast<uint32>(std::filesystem::file_size(InPath));
}

String Utility::File::GetCachePath(const String &InPath, String InExt)
{
    if (!InExt.contains("."))
        InExt = "." + InExt;
    return "Cache/" + ToStr(Hash(InPath)) + InExt; 
}

String Utility::File::Relative(const String &InPath)
{
    auto current = std::filesystem::current_path();
    auto currStr = current.string();
    if (InPath.size() < currStr.size())
        return InPath;
    if (InPath == currStr)
        return {};
    auto find = InPath.find_first_of(currStr);
    if (find == String::npos)
        return InPath; // Already relative
    auto result = InPath.substr( find + currStr.length() + 1);
    return result;
}

String Utility::File::Name(const String &InPath)
{
    auto back = InPath.find_last_of('\\');
    auto forw = InPath.find_last_of('/');
    if (back == std::string::npos && forw == std::string::npos)
        return InPath; // At root
    auto slash = Math::Max(
        back == std::string::npos ? 0 : back,
        forw == std::string::npos ? 0 : forw);
    return InPath.substr(slash + 1);
}
