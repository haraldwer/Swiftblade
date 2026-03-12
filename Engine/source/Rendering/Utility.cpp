#include "Utility.h"

#include "webgpu/webgpu.h"

const char *Rendering::ToStr(const WGPUStringView &InStr)
{
    if (InStr.data)
        return InStr.data;
    return "";
}

WGPUStringView Rendering::ToStr(const String &InStr)
{
    return WGPUStringView(InStr.c_str());
}
