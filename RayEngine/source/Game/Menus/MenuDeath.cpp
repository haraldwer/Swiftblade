#include "MenuDeath.h"

#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Transform.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Game/GameInstance.h"
#include "Game/ECS/Player/Input.h"

void MenuDeath::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
            .Push(UI::List(UI::Transform::Fill(10.0f), 10.0f), "List")
                .Add(UI::Label("Death", 0.5f, UI::Transform::Fill(), ResFont("F_GothBallCrap.ttf"), 80.0f))
                .Add(UI::Label("Retry", 0.5f), "Retry")
                .Add(UI::Label("Main Menu", 0.5f), "Main Menu");
    
    UI = builder.Build();

    // Disable player input and show mouse
    ECS::Input::Blocked = true;
    if (IsCursorHidden())
        ShowCursor();

    // Freeze the game
    Utility::Time::Get().SetPause(true);
}

void MenuDeath::Update(double InDelta)
{
    if (UI.Get<UI::Label>("Retry").IsClicked())
    {
        // Push new game instance
        Engine::Manager::Get().Pop();
        Engine::Manager::Get().Push<GameInstance>();
    }

    if (UI.Get<UI::Label>("Main Menu").IsClicked())
        Engine::Manager::Get().Pop();
}
