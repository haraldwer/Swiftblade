#pragma once

#if defined(TRACY_ENABLE)

#include "tracy/Tracy.hpp"

#define RN_PROFILE_INIT() //TracyGpuContext
#define RN_PROFILE_COLLECT() //TracyGpuCollect
#define RN_PROFILE_GPU(x) //TracyGpuZone(x)
#define RN_PROFILE() ZoneScopedC(tracy::Color::ColorType::DarkSlateGray)
#define RN_PROFILE_NAMED(x) ZoneScopedNC(x, tracy::Color::ColorType::DarkSlateGray)

#else

#define RN_PROFILE_INIT()
#define RN_PROFILE_COLLECT()
#define RN_PROFILE_GPU(x)
#define RN_PROFILE()
#define RN_PROFILE_NAMED(x)

#endif