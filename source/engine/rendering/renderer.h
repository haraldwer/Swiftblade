#pragma once

#include <vector>

#include "RenderScene.h"
#include "Utility/Singelton.h"

inline constexpr int ScreenWidth  = 800;
inline constexpr int ScreenHeight = 450;

class Renderer : public Singelton<Renderer>
{

public:

    void Init() override; 
    void Render();

    void ClearScenes(); 
    void PushScene(const RenderScene& InScene);

private:

    std::vector<RenderScene> Scenes; 

};
