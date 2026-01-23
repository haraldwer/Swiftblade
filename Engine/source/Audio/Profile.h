#pragma once

#ifdef TRACY_ENABLE

#include "tracy/Tracy.hpp"

#define AU_PROFILE() ZoneScopedC(tracy::Color::ColorType::DarkOrchid)
#define AU_PROFILE_NAMED(x) ZoneScopedNC(x, tracy::Color::ColorType::DarkOrchid)

#else

#define AU_PROFILE()
#define AU_PROFILE_NAMED(x)

#endif