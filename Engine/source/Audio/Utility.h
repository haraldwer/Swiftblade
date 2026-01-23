#pragma once

#define AU_LOG(text) LOG(AU, text)
#define AU_RETURN_LOG(condition, text, ...) CHECK_RETURN_LOG(AU, text, __VA_ARGS__)
#define AU_BREAK_LOG(condition, text) CHECK_BREAK_LOG(AU, text)
#define AU_CONTINUE_LOG(condition, text) CHECK_CONTINUE_LOG(AU, text)