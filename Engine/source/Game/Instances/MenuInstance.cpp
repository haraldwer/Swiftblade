#include "MenuInstance.h"

#include "../UI/Menus/MenuMain.h"
#include "Instance/Manager.h"
#include "Scene/SceneResource.h"
#include "UI/Menus/MenuLogin.h"

void MenuInstance::Init()
{
    Instance::Init();

#ifdef NAKAMA_ENABLE
    menus.Push<MenuLogin>();
#else
    menus.Push<MenuMain>();
#endif
    
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
        Engine::Manager::Get().Pop();
}

void MenuInstance::Frame()
{
    Rendering::EnvironmentInstance env;
    env.skybox = config.Skybox;
    GetRenderScene().AddEnvironment(env);
    ecs.Frame(); 
    Instance::Frame();
}
