#pragma once

#include "ECS/Manager.h"
#include "Engine/Instance/Instance.h"
#include "Scene/SceneResource.h"

struct MenuConfig : BaseConfig<MenuConfig>
{
    PROPERTY_D(ResScene, Scene, "Scenes/S_GameStart.json")
    PROPERTY(Vec3F, CameraPosition);
    PROPERTY(Vec3F, CameraRotation);
    PROPERTY_C(ResRM, Skybox, "Dressing/Skybox/RM_Skybox.json");

    String Name() const override { return "Menu"; }
};

class MenuInstance : public Engine::Instance 
{
    TYPE_INFO(MenuInstance, Engine::Instance);
    
public: 
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;
    bool IsEditor() const override { return true; }
    
private:
    ECS::Manager ecs = {}; 
    SceneInstance scene = {};
    MenuConfig config = {};
};
