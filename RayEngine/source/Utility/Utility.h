#pragma once

#define CONCAT_INTERNAL(A, B) A ## B
#define CONCAT(A, B) CONCAT_INTERNAL(A, B)

namespace Utility
{
    void ExternalLog(const String& InStr);
    String DeviceGUID();
}

#define LOG_CONSOLE(text) { Utility::ExternalLog(String(__FUNCTION__) + "::" + Utility::ToStr(__LINE__) + " | " + Utility::ToStr(text)); }
#define LOG(text) { LOG_CONSOLE(text); }

#define CHECK_ASSERT(condition, text) \
assert(!(condition) && text);

#define CHECK_RETURN(condition, ...) \
if (condition) { return __VA_ARGS__; }

#define CHECK_RETURN_LOG(condition, text, ...) \
if (condition) [[unlikely]] { LOG(text); return __VA_ARGS__; }

#define CHECK_BREAK(condition) \
if (condition) { break; }

#define CHECK_BREAK_LOG(condition, text) \
if (condition) [[unlikely]] { LOG(text); break; }

#define CHECK_CONTINUE(condition) \
if (condition) { continue; }

#define CHECK_CONTINUE_LOG(condition, text) \
if (condition) [[unlikely]] { LOG(text); continue; }
