#pragma once
#include "webgpu/webgpu.h"

namespace Rendering
{
    const char* ToStr(const WGPUStringView & InStr);
    WGPUStringView ToStr(const String& InStr);
}
