#pragma once

namespace Profiling
{
    void BeginScope(const String& InName, const String& InFunction);
    void EndScope();
}

#ifdef _DEBUG
#define PROFILE_SCOPE_BEGIN(name) { Profiling::BeginScope(String(name), String(__FUNCTION__) + "::" + std::to_string(__LINE__)); }
#define PROFILE_SCOPE_END() { Profiling::EndScope(); }
#else
#define PROFILE_SCOPE_BEGIN(name)
#define PROFILE_SCOPE_END()
#endif