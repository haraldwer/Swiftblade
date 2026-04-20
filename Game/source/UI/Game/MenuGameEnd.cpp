#include "MenuGameEnd.h"

#include "Engine/Instance/InstanceManager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Transform.h"
#include "Instances/GameInstance.h"

void MenuGameEnd::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder();
    ui = builder.Build();

    // Disable player input and show mouse
    Input::Manager::Get().Push("Default"); 

    // Freeze the game
    Utility::Time::Get().SetPause(true);
}

void MenuGameEnd::Deinit()
{
    Instance::Deinit();
    Input::Manager::Get().Pop("Default"); 
}

void MenuGameEnd::Update()
{
    if (ui["Replay"].IsClicked())
    {
        // Push new game instance
        Engine::InstanceManager::Get().Pop();
        Engine::InstanceManager::Get().Push<GameInstance>();
    }

    if (ui["Main Menu"].IsClicked())
        Engine::InstanceManager::Get().Pop();
}
