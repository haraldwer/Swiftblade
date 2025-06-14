﻿#include "MenuGameEnd.h"

#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Transform.h"
#include "GameInstance.h"

void MenuGameEnd::Init()
{
    // Create the UI instance
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
            .Push(UI::List(UI::Transform::Fill(10.0f), 10.0f), "List")
                .Add(UI::Label("End", 0.5f, UI::Transform::Fill(), ResFont("F_GothBallCrap.ttf"), 80.0f));


    const int score = static_cast<int>(GameState::Get().elapsedTime);
    builder.Add(UI::Label("Score: " + std::to_string(score), 0.5f), "Score");
    builder.Add(UI::Label("Replay", 0.5f), "Replay");
    builder.Add(UI::Label("Main Menu", 0.5f), "Main Menu");
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
        Engine::Manager::Get().Pop();
        Engine::Manager::Get().Push<GameInstance>();
    }

    if (ui["Main Menu"].IsClicked())
        Engine::Manager::Get().Pop();
}
