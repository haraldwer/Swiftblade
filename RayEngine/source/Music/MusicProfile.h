#pragma once

#ifdef TRACY_ENABLE

#include "tracy/Tracy.hpp"

#define PROFILE_AU() ZoneScopedC(tracy::Color::ColorType::DarkOrchid)
#define PROFILE_AU_NAMED(x) ZoneScopedNC(x, tracy::Color::ColorType::DarkOrchid)

#else

#define PROFILE_GL()
#define PROFILE_GL_NAMED(x)

#endif