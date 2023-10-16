#pragma once
#include "raylib.h"
#include "utility/singelton.h"

class CameraManager : public Singelton<CameraManager>
{
    
public:
    
    void Init() override;
    void SetCamera(const Camera& InCamera) { CurrentCamera = InCamera; }
    Camera& GetCamera() { return CurrentCamera; }

private:

    Camera CurrentCamera = {};
    
};
