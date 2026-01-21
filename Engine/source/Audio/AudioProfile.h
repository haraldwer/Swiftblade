#pragma once

#ifdef TRACY_ENABLE

#include "tracy/Tracy.hpp"

#define PROFILE_AU() ZoneScopedC(tracy::Color::ColorType::DarkOrchid)
#define PROFILE_AU_NAMED(x) ZoneScopedNC(x, tracy::Color::ColorType::DarkOrchid)

#else

#define PROFILE_AU()
#define PROFILE_AU_NAMED(x)

#endif