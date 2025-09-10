#include "MenuDeath.h"

#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Transform.h"
#include "Instances/GameInstance.h"

void MenuDeath::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder();
    
    ui = builder.Build();
    
    // Disable player input and show mouse
    Input::Manager::Get().Push("Default");

    // Freeze the game
    Utility::Time::Get().SetPause(true);
}

void MenuDeath::Deinit()
{
    Instance::Deinit();
    Input::Manager::Get().Pop("Default"); 
}

void MenuDeath::Update()
{
    if (const UI::Label* respawn = ui.TryGet<UI::Label>("Respawn"))
    {
        if (respawn->IsClicked())
        {
            // Push new game instance
            Engine::Manager::Get().Pop();
            if (const auto newGame = Engine::Manager::Get().Push<GameInstance>())
                newGame->SetState(GameState::Get()); // Transfer game state 
        }
    }
    
    //if (ui["Main Menu"].IsClicked())
    //    Engine::Manager::Get().Pop();
}
