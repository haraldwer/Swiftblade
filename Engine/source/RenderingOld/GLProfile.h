#pragma once

#if defined(TRACY_ENABLE)

#include "tracy/Tracy.hpp"

#define PROFILE_GL_INIT() //TracyGpuContext
#define PROFILE_GL_COLLECT() //TracyGpuCollect
#define PROFILE_GL_GPU(x) //TracyGpuZone(x)
#define PROFILE_GL() ZoneScopedC(tracy::Color::ColorType::DarkSlateGray)
#define PROFILE_GL_NAMED(x) ZoneScopedNC(x, tracy::Color::ColorType::DarkSlateGray)

#else

#define PROFILE_GL_INIT()
#define PROFILE_GL_COLLECT()
#define PROFILE_GL_GPU(x)
#define PROFILE_GL()
#define PROFILE_GL_NAMED(x)

#endif