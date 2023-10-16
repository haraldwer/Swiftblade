#include "renderer.h"

#include "raylib.h"
#include "rlgl.h"
#include "game/game.h"

void Renderer::Init()
{
    CameraManager.Init(); 
    
    InitWindow(50, 50, "rayengine");
    SetTargetFPS(60);

    SetWindowSize(ScreenWidth, ScreenHeight);
}

void Renderer::Render()
{
    auto& game = Game::Get();
    auto& camera = CameraManager::Get().GetCamera();
    UpdateCamera(&camera, CAMERA_FREE);

    BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
            rlPushMatrix();

                // Draw custom shape
    
                rlCheckRenderBatchLimit(game.Room.Vertices.size());

                rlBegin(RL_TRIANGLES);
                Color color = GREEN;
                rlColor4ub(color.r, color.g, color.b, color.a);
                for (auto& vert : game.Room.Vertices)
                    rlVertex3f(vert.x, vert.y, vert.z);
    
            rlPopMatrix();
        EndMode3D();
    EndDrawing();
}
