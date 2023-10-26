#include "raylib.h"
#include "Engine/Physics/Manager.h"
#include "engine/rendering/renderer.h"
#include "Engine/Resource/Manager.h"
#include "game/game.h"

int main()
{
    // Renderer and resource manager are shared between instances
    Rendering::Renderer renderer;
    Resource::Manager resourceManager;
    renderer.Init();

    // A game instance that can be created / destroyed at any time :) 
    Game game;
    game.Init();
    
    constexpr double tickRate = 300.0; 
    constexpr double fixedDelta = 1.0 / tickRate;
    double tickTimer = 0.0;
    
    while (true)
    {
        if (WindowShouldClose())
            break;

        const double delta = GetFrameTime();

        // Fixed update
        tickTimer += delta;
        while (tickTimer >= 0)
        {
            tickTimer -= fixedDelta;
            renderer.Clear();
            game.Update(fixedDelta);
        }

        // Render
        renderer.Render(delta);
    }
    
    game.Deinit();

    CloseWindow();
    return 0;
}