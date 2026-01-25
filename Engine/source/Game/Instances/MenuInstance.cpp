#include "MenuInstance.h"

#include "../UI/Menus/MenuMain.h"
#include "ECS/Registration.h"
#include "Game/ECS/Registration.h"
#include "Instance/InstanceManager.h"
#include "Scene/SceneResource.h"
#include "Scene/Instances/EnvironmentInstance.h"
#include "UI/Menus/MenuLogin.h"

void MenuInstance::Init()
{
    Instance::Init();

#ifdef NAKAMA_ENABLE
    menus.Push<MenuLogin>();
#else
    menus.Push<MenuMain>();
#endif
    
    ECS::RegisterEngineSystems();
    ECS::RegisterGameSystems();
    ecs.Init();
    
    config.LoadConfig();
    if (auto ptr = config.Scene.Get().Get())
        scene = ptr->Instantiate();

    freeCamera.Toggle();
    freeCamera.SetState(
        config.CameraPosition,
        config.CameraRotation);
}

void MenuInstance::Deinit()
{
#ifdef _DEBUG
    config.CameraPosition = freeCamera.GetPosition();
    config.CameraRotation = freeCamera.GetRotation();
    config.SaveConfig();
#endif

    scene.Destroy();
    ecs.Deinit();
    Instance::Deinit();
}

void MenuInstance::Logic(const double InDelta)
{
    Instance::Logic(InDelta);
    
    ecs.Update();
    freeCamera.Update();
    
    if (Input::Action::Get("Back").Pressed())
        menus.Pop();

    if (menus.Empty())
        Engine::InstanceManager::Get().Pop();
}

void MenuInstance::Frame()
{
    Rendering::EnvironmentInstance env;
    env.skybox = config.Skybox;
    // TODO: GetRenderScene().AddEnvironment(env);
    ecs.Frame(); 
    Instance::Frame();
}
