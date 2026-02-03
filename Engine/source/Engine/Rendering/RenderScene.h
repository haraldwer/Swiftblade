#pragma once
#include "Scene/Scene.h"
#include "Utility/Singelton.h"

namespace Engine
{
    struct RenderScene : Rendering::Scene, Utility::Singelton<RenderScene> { };
}
