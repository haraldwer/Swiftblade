#include "game.h"

#include "raylib.h"
#include "engine/rendering/camera/camera_manager.h"

void Game::Init()
{
    Singelton::Init();

    Camera camera;
    camera.position = Vector3{ 16.0f, 16.0f, 16.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 90.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    CameraManager::Get().SetCamera(camera);

    Room.Vertices.push_back(Vector3(0.0f, 0.0f, 0.0f));
    Room.Vertices.push_back(Vector3(1.0f, 0.0f, 0.0f));
    Room.Vertices.push_back(Vector3(1.0f, 1.0f, 0.0f));
    Room.Vertices.push_back(Vector3(0.0f, 1.0f, 0.0f));
}

void Game::Update()
{
    TickTimer += GetFrameTime();
    while (TickTimer > TickRate)
    {
        TickTimer -= TickRate;
        FixedUpdate(TickRate);
    }
}

void Game::FixedUpdate(double InDelta)
{
}
