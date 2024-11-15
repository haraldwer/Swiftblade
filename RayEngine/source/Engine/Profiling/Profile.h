#pragma once

namespace Profiling
{
    void BeginScope(const String& InName, const String& InFunction);
    void EndScope();
}

#define PROFILE_SCOPE_BEGIN(name) { Profiling::BeginScope(String(name), String(__FUNCTION__) + "::" + std::to_string(__LINE__)); }
#define PROFILE_SCOPE_END() { Profiling::EndScope(); }