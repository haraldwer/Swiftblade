#include "Game.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"

void Game::Init()
{
    Instance::Init();
    Physics.Init();


    //if (BlueprintResource* bp = ResBlueprint("Blueprints/BP_Cube.json").Get())
    //    bp->Instantiate();
    
    if (BlueprintResource* bp = ResBlueprint("Blueprints/BP_Player.json").Get())
        bp->Instantiate();

    //if (BlueprintResource* bp = ResBlueprint("Blueprints/BP_PhysCube.json").Get())
    //    bp->Instantiate(); 
}

void Game::Deinit()
{
    Instance::Deinit();
    Physics.Deinit();
}

void Game::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    const double scaledDelta = Time.Delta();
    Physics.Update(scaledDelta);
    ECS.Update(scaledDelta);

    if (IsKeyDown(KEY_LEFT_CONTROL))
        if (IsKeyPressed(KEY_P))
            Engine::Manager::Get().Pop();
}
