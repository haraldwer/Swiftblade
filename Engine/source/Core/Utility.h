#pragma once

#define UT_LOG(text) LOG(UT, text)
#define UT_RETURN_LOG(condition, text, ...) CHECK_RETURN_LOG(UT, text, __VA_ARGS__)
#define UT_BREAK_LOG(condition, text) CHECK_BREAK_LOG(UT, text)
#define UT_CONTINUE_LOG(condition, text) CHECK_CONTINUE_LOG(UT, text)
