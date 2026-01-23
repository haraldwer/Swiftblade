#pragma once

#include "glfw3webgpu.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "webgpu/webgpu.hpp"

#include "Core/pch.h"
#include "Utility.h"
#include "Profile.h"

inline const char* ToStr(const wgpu::StringView& InStr)
{
    if (InStr.data)
        return InStr.data;
    return "";
}
