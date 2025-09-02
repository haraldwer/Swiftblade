#include "MenuInstance.h"

#include "../UI/Menus/MenuMain.h"
#include "Instance/Manager.h"
#include "Scene/SceneResource.h"
#include "UI/Menus/MenuLogin.h"

void MenuInstance::Init()
{
    Instance::Init();
    menus.Push<MenuLogin>();
    ecs.Init();
    db.Init();
    
    config.LoadConfig();
    if (auto ptr = config.Scene.Get().Get())
        scene = ptr->Instantiate();

    editorCamera.Toggle();
    editorCamera.SetState(
        config.CameraPosition,
        config.CameraRotation);
}

void MenuInstance::Deinit()
{
#ifdef _DEBUG
    config.CameraPosition = editorCamera.GetPosition();
    config.CameraRotation = editorCamera.GetRotation();
    config.SaveConfig();
#endif

    scene.Destroy();
    ecs.Deinit();
    Instance::Deinit();
    db.Deinit();
}

void MenuInstance::Logic(const double InDelta)
{
    Instance::Logic(InDelta);
    
    db.Update();
    ecs.Update();
    editorCamera.Update();
    
    if (Input::Action::Get("Back").Pressed())
        menus.Pop();

    if (menus.Empty())
        Engine::Manager::Get().Pop();
}

void MenuInstance::Frame()
{
    EnvironmentInstance env;
    env.skybox = config.Skybox;
    GetRenderScene().AddEnvironment(env);
    ecs.Frame(); 
    Instance::Frame();
}
