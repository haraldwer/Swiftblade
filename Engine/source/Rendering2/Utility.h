#pragma once

#define RN_LOG(text) LOG(RN, text)
#define RN_RETURN_LOG(condition, text, ...) CHECK_RETURN_LOG(RN, text, __VA_ARGS__)
#define RN_BREAK_LOG(condition, text) CHECK_BREAK_LOG(RN, text)
#define RN_CONTINUE_LOG(condition, text) CHECK_CONTINUE_LOG(RN, text)
