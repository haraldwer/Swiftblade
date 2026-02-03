#include "FreeCamera.h"

#include "Input/Action.h"
#include "Instance/Instance.h"
#include "Scene/Instances/CameraInstance.h"

void Engine::FreeCamera::Update()
{
    PROFILE();
    
    if (Input::Action::Get("EditorCamera").Pressed())
    {
        if (bAlwaysEnabled)
            bRequireHold = !bRequireHold; 
        else
            Toggle();
    }

    const bool previousEnableInput = bInputEnabled;
    const Input::Action& rm = Input::Action::Get("RM");
    bInputEnabled = bUseEditCamera && (!bRequireHold || rm.Down());  
    if (bInputEnabled != previousEnableInput)
    {
        if (bInputEnabled)
        {
            Input::Manager::Get().Push("EditorCamera");
        }
        else
        {
            Input::Manager::Get().Pop("EditorCamera");
        }
    }

    if (bUseEditCamera)
        UpdateMovement(); 
}

void Engine::FreeCamera::Deinit()
{
    if (bInputEnabled)
        Input::Manager::Get().Pop("EditorCamera");
    bInputEnabled = false;
    if (bUseEditCamera)
        Input::Manager::Get().Pop("Default");
    bUseEditCamera = false;
}

void Engine::FreeCamera::SetState(const Vec3F &InPos, const Vec3F &InRot)
{
    targetState.position = InPos;
    targetState.rotation = InRot;
    currentState = targetState;
}

void Engine::FreeCamera::SetTarget(const Vec3F& InPosition, const float InDistance)
{
    const Vec3F dir = Vec3F(1.0f, -1.0f, 1.0f).GetNormalized();
    targetState.position = InPosition - dir * InDistance;
    targetState.rotation = {
        PI_FLOAT * 0.2,
        PI_FLOAT / 4,
        0
    };
    currentState = targetState;
}

Vec3F Engine::FreeCamera::GetMouseDirection() const
{
    /*
    Vector2 mouse = GetMousePosition();
    return ScreenToWorld({ mouse.x, mouse.y });
    */
    return {};
}

Vec3F Engine::FreeCamera::ScreenToWorld(const Vec2F &InScreen) const
{
    return Vec3F();
    // TODO: 
    /*
    auto& man = Rendering::PrevManager::Get();
    auto& view = man.mainViewport;
    auto viewPos = view.DistortCoord(view.ScreenToViewportAbsolute(InScreen));
    
    Vec2F clipPos = viewPos * 2 - 1;
    return ClipToWorld(clipPos);
    */
}

Vec3F Engine::FreeCamera::ClipToWorld(Vec2F InClip) const
{
    Rendering::CameraInstance c {
        currentState.position,
         QuatF::FromEuler(currentState.rotation),
         currentState.fov,
         currentState.far,
         currentState.near,
    };
    return Vec3F();
    // TODO:
    /*
    auto& view = Rendering::PrevManager::Get().mainViewport;
    auto size = view.GetSize();
    Vec4F clip = { InClip.x, -InClip.y, -1, 1 };
    Vec4F rayEye = Mat4F::Transpose(Mat4F::GetInverse(c.GetProjectionMatrix(size.To<float>()))) * clip;
    rayEye = { rayEye.x, rayEye.y, 1, 1};
    Vec4F world = Mat4F::Transpose(Mat4F::GetInverse(c.GetViewMatrix())) * rayEye;
    return world.xyz.GetNormalized();
    */
}

void Engine::FreeCamera::Toggle()
{
    bUseEditCamera = !bUseEditCamera;
    if (bUseEditCamera)
    {
        auto& cam = RenderScene::Get().camera;
        if (cam.fov > 1.0f)
        {
            targetState.position = cam.position;
            targetState.rotation = cam.rotation.Euler();
            targetState.fov = cam.fov;
        }
        currentState = targetState;
        Input::Manager::Get().Push("Default");
    }
    else
    {
        Input::Manager::Get().Pop("Default");
    }
}

void Engine::FreeCamera::UpdateMovement()
{
    PROFILE();
    
    float dt = static_cast<float>(Utility::Time::Get().Unscaled());
    
    auto& man = Input::Manager::Get();
    const bool ctrl = man.GetAction("Control", "EditorCamera").Down(); 
    
    // Calculate rotation
    const Vec2F mouseDelta = {
        man.GetAction("LookHorizontal", "EditorCamera").Axis(),
        man.GetAction("LookVertical", "EditorCamera").Axis()
    };
    targetState.rotation +=
        Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.0025f * !ctrl;
    targetState.rotation.x = Utility::Math::Clamp(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        targetState.rotation.x);  

    const Mat4F rotMat = Mat4F::FromEuler(targetState.rotation);
    const Vec3F up = Vec3F::Up();
    const Vec3F right = rotMat.Right() * -1.0f;
    const Vec3F forward = rotMat.Forward();
    
    const float scrollDelta = 0;// TODO: GetMouseWheelMove() * 0.1f;
    targetState.movementSpeed = Utility::Math::Clamp(targetState.movementSpeed + scrollDelta * (50.0f + targetState.movementSpeed * 0.5f), 1.0f, 300.0f);
    
    // Add position
    const Vec3F posDelta =
        (up * (
            static_cast<float>(man.GetAction("Up", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("Down", "EditorCamera").Down())) +
        right * (
            static_cast<float>(man.GetAction("Right", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("Left", "EditorCamera").Down())) +
        forward * (
            static_cast<float>(man.GetAction("Forward", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("Backward", "EditorCamera").Down()))) * 
        !ctrl;
    targetState.position += Vec3F(posDelta.GetNormalized()) * dt * targetState.movementSpeed;
    
    // FOV
    targetState.fov += (
            static_cast<float>(man.GetAction("IncreaseFOV", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("DecreaseFOV", "EditorCamera").Down())) *
        static_cast<float>(!ctrl) * 
        dt * 30.0f;

    // TODO: Interp account for dt
    currentState.rotation = targetState.rotation;
    currentState.position = Utility::Math::Lerp(currentState.position, targetState.position, 10.0f * dt);
    currentState.fov = Utility::Math::Lerp(currentState.fov, targetState.fov, 10.0f * dt);
    currentState.movementSpeed = Utility::Math::Lerp(currentState.movementSpeed, targetState.movementSpeed, 10.0f * dt); 
    
    // Set camera
    RenderScene::Get().camera = {
       currentState.position,
        QuatF::FromEuler(currentState.rotation),
        currentState.fov,
        50.0f,
        0.01f
   };
}
