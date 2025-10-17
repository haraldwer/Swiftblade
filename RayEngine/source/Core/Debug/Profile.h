#pragma once

#ifdef TRACY_ENABLE 

#include "tracy/Tracy.hpp"

#define PROFILE_FRAME() FrameMark
#define PROFILE() ZoneScoped
#define PROFILE_NAMED(x) ZoneScopedN(x)
#define PROFILE_TAG(x, y) ZoneText(x, strlen(x))
#define PROFILE_LOG(text, size) TracyMessage(text, size)
#define PROFILE_VALUE(text, value) TracyPlot(text, value)

#else

#define PROFILE_FRAME()
#define PROFILE()
#define PROFILE_NAMED(x)
#define PROFILE_TAG(x, y)
#define PROFILE_LOG(text, size)
#define PROFILE_VALUE(text, value)

#endif