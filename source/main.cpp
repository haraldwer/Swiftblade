
#include "raylib.h"
#include "engine/rendering/renderer.h"
#include "game/game.h"

int main()
{
    Renderer r;
    Game g; 
    r.Init();
    g.Init();

    while (true)
    {
        if (WindowShouldClose())
            break;
        
        g.Update();
        r.Render();
    }

    CloseWindow();
    return 0;
}