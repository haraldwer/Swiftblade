#pragma once
#include "camera/camera_manager.h"

inline constexpr int ScreenWidth  = 800;
inline constexpr int ScreenHeight = 450;

class Renderer
{

public:

    void Init(); 
    void Render();

private:

    CameraManager CameraManager;
    
};
