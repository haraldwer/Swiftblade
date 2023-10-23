
#include "raylib.h"
#include "engine/rendering/renderer.h"
#include "Engine/Resource/ResourceManager.h"
#include "game/game.h"

int main()
{
    Rendering::Renderer renderer;
    ResourceImpl::ResourceManager resourceManager;
    Game game;
    renderer.Init();
    game.Init();

    while (true)
    {
        if (WindowShouldClose())
            break;
        
        game.Update();
        renderer.Render();
    }

    CloseWindow();
    return 0;
}