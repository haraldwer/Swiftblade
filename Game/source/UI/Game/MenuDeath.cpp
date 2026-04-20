#include "MenuDeath.h"

#include "Engine/Instance/InstanceManager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Transform.h"
#include "Instances/GameInstance.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelTitle.h"
#include "UI/Widgets/Common/Separator.h"

void MenuDeath::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder()
        .Push(UI::List(UI::Transform::Centered(), { 5 }))
            .Add(UI::LabelTitle(UI::Transform::Centered(), "Death"))
            .Add(UI::Separator())
            .Add(UI::ButtonDefault(UI::Transform::Centered(), { "Respawn" }), "Respawn")
            .Add(UI::ButtonDefault(UI::Transform::Centered(), { "Main Menu" }), "Main Menu");
    
    ui = builder.Build();
    
    // Disable player input and show mouse
    Input::Manager::Get().Push("Default");

    // Freeze the game
    Utility::Time::Get().SetPause(true);

    // Maybe disable respawn button?
}

void MenuDeath::Deinit()
{
    Instance::Deinit();
    Input::Manager::Get().Pop("Default"); 
}

void MenuDeath::Update()
{
    Instance::Update();
    
    if (ui["Respawn"].IsClicked())
    {
        // Push new game instance
        Engine::InstanceManager::Get().Pop();
        if (const auto newGame = Engine::InstanceManager::Get().Push<GameInstance>())
            newGame->SetState(GameState::Get()); // Transfer game state
    }
    
    if (ui["Main Menu"].IsClicked())
        Engine::InstanceManager::Get().Pop();
}
