#include "RenderInstance.h"

#include "Instance/InstanceManager.h"
#include "Scene/Instances/EnvironmentInstance.h"
#include "Scene/Instances/MeshInstance.h"

void RenderInstance::Init()
{
    Instance::Init();
    
    for (auto& f : Utility::File::List("Sponza"))
    {
        CHECK_CONTINUE(!f.ends_with(".obj"))
        LOG("Sponza model: " + f);
        auto m = ResModel(f);
        sponza.push_back(m);
    }
    // TODO: 
    /*
    auto& rs = GetRenderScene();
    for (const auto& m : sponza)
        rs.Meshes().Add({
                m.model,
                material,
                Mat4F(), 
                m.hash,
                static_cast<uint8>(Rendering::VisibilityMask::ALL)
            }, persistantID);
    */
    
    freeCamera.Toggle();
    freeCamera.SetState(Vec3F::Zero(), Vec3F::Zero());
}

void RenderInstance::Logic(double InDelta)
{
    Instance::Logic(InDelta);
    
    freeCamera.Update();
    if (Input::Action::Get("Back").Pressed())
        Engine::InstanceManager::Get().Pop();
}

void RenderInstance::Frame()
{
    Rendering::EnvironmentInstance env;
    env.skybox = skybox;
    //GetRenderScene().AddEnvironment(env);
    Instance::Frame();
}

void RenderInstance::DrawPanel()
{
    ImGui::Text("Hi, this is sponzaaa!");
}
