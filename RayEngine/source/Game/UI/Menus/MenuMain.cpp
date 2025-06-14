﻿#include "MenuMain.h"

#include "Editor/RoomEditor.h"
#include "Engine/Editor/BlueprintEditor.h"
#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Image.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Elements/RectImage.h"
#include "GameInstance.h"
#include "MenuLeaderboard.h"

void MenuMain::Init()
{
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
            .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
            .Push(UI::List(UI::Transform::Fill(10.0f), 10.0f), "List")
                .Add(UI::Label("Swiftblade", 0.5f, UI::Transform::Fill(), ResFont("F_GothBallCrap.ttf"), 80.0f));
    
    const Vector<String> buttons = {
        "Play",
        "Leaderboard",
        "Create",
        "Edit"
    };

    UI::Container buttonWidget = UI::Builder()
       .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
       .Add(UI::Label("Text", 0.5f), "Text")
        .Build();
    
    for (const String& b : buttons)
    {
        buttonWidget.Get<UI::Label>("Text").SetText(b);
        builder.Add(buttonWidget, b);
    }
    
    ui = builder.Build();
}

void MenuMain::Update()
{
    if (ui["Play"].IsClicked())
        Engine::Manager::Get().Push<GameInstance>();
    if (ui["Leaderboard"].IsClicked())
        Menu::Manager::Get().Push<MenuLeaderboard>();
    if (ui["Create"].IsClicked())
        Engine::Manager::Get().Push<RoomEditor>();
    if (ui["Edit"].IsClicked())
        Engine::Manager::Get().Push<BlueprintEditor>(); 
}
