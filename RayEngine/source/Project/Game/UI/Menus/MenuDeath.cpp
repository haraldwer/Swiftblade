#include "MenuDeath.h"

#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Transform.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Game/GameInstance.h"
#include "Game/ECS/Player/PlayerInput.h"

void MenuDeath::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
            .Push(UI::List(UI::Transform::Fill(10.0f), 10.0f), "List")
                .Add(UI::Label("Death", 0.5f, UI::Transform::Fill(), ResFont("F_GothBallCrap.ttf"), 80.0f));

    if (GameState::Get().Deaths < 3)
        builder.Add(UI::Label("Respawn", 0.5f), "Respawn");
    builder.Add(UI::Label("Main Menu", 0.5f), "Main Menu");
    
    UI = builder.Build();
    
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
    CHECK_RETURN(!UI);

    if (const UI::Label* respawn = UI->TryGet<UI::Label>("Respawn"))
    {
        if (respawn->IsClicked())
        {
            // Push new game instance
            Engine::Manager::Get().Pop();
            if (const auto newGame = Engine::Manager::Get().Push<GameInstance>())
                newGame->SetState(GameState::Get()); // Transfer game state 
        }
    }
    
    if (UI->Get<UI::Label>("Main Menu").IsClicked())
    {
        Engine::Manager::Get().Pop();
    }
}
