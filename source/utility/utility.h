#pragma once

#include <assert.h>
#include "Log/Log.h"

#define CONCAT_INTERNAL(A, B) A ## B
#define CONCAT(A, B) CONCAT_INTERNAL(A, B)

#define LOG_CONSOLE(text) \
Log(text);
#define LOG(text) \
LOG_CONSOLE(text);

#define CHECK_ASSERT(condition, text) \
assert(!(condition) && text);

#define CHECK_RETURN(condition, ...) \
if (condition) { return __VA_ARGS__; }

#define CHECK_RETURN_LOG(condition, text, ...) \
if (condition) { LOG(text); return __VA_ARGS__; }

#define CHECK_BREAK(condition) \
if (condition) { break; }

#define CHECK_BREAK_LOG(condition, text) \
if (condition) { LOG(text); break; }

#define CHECK_CONTINUE(condition) \
if (condition) { continue; }

#define CHECK_CONTINUE_LOG(condition, text) \
if (condition) { LOG(text); continue; }
