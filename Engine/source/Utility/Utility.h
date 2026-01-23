#pragma once

#include "Logging.h"

#define CONCAT_INTERNAL(A, B) A ## B
#define CONCAT(A, B) CONCAT_INTERNAL(A, B)

#define LOG(category, text) { Utility::ExternalLog(#category, __FILE__, __FUNCTION__, __LINE__, Utility::ToStr(text)); }

#define CHECK_ASSERT(condition, text) \
assert(!(condition) && text);

#define CHECK_RETURN(condition, ...) \
if (condition) { return __VA_ARGS__; }

#define CHECK_RETURN_LOG(category, text, condition, ...) \
if (condition) [[unlikely]] { LOG(category, text); return __VA_ARGS__; }

#define CHECK_BREAK(condition) \
if (condition) { break; }

#define CHECK_BREAK_LOG(category, text, condition) \
if (condition) [[unlikely]] { LOG(category, text); break; }

#define CHECK_CONTINUE(condition) \
if (condition) { continue; }

#define CHECK_CONTINUE_LOG(category, text, condition) \
if (condition) [[unlikely]] { LOG(category, text); continue; }
