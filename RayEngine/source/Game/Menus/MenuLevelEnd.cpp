#include "MenuLevelEnd.h"

#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Transform.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Game/GameInstance.h"
#include "Game/ECS/Player/Input.h"

void MenuLevelEnd::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
            .Push(UI::List(UI::Transform::Fill(10.0f), 10.0f), "List")
                .Add(UI::Label("End", 0.5f, UI::Transform::Fill(), ResFont("F_GothBallCrap.ttf"), 80.0f))
                .Add(UI::Label("Replay", 0.5f), "Replay")
                .Add(UI::Label("Main Menu", 0.5f), "Main Menu");
    
    UI = builder.Build();

    // Disable player input and show mouse
    ECS::Input::Blocked = true;
    if (IsCursorHidden())
        ShowCursor(); 
}

void MenuLevelEnd::Update(double InDelta)
{
    if (UI.Get<UI::Label>("Replay").IsClicked())
    {
        // Push new game instance
        Engine::Manager::Get().Pop();
        Engine::Manager::Get().Push<GameInstance>();
    }

    if (UI.Get<UI::Label>("Main Menu").IsClicked())
        Engine::Manager::Get().Pop();
}
