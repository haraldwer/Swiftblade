#include "Utility.h"

#include <webgpu/webgpu.hpp>

const char *Rendering::ToStr(const wgpu::StringView &InStr)
{
    if (InStr.data)
        return InStr.data;
    return "";
}
