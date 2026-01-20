#pragma once

#include "Core/pch.h"
#include "GLProfile.h"

// TODO: Custom log category
#define GL_LOG(text) { Utility::ExternalLog(__FILE__, __FUNCTION__, __LINE__, Utility::ToStr(text)); }

namespace Rendering
{
    typedef Utility::Color Color;
}