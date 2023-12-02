#include "MenuMain.h"

#include "Editor/BlueprintEditor.h"
#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "game/GameInstance.h"
#include "Game/Editor/RoomEditor.h"

void MenuMain::Init()
{
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(600.0f, -300.0f, 0.5f)))
            .Push(UI::List(UI::Transform::Fill(10.0f), 10.0f), "List")
                .Add(UI::Label("Swiftblade", 0.5f, UI::Transform::Fill(), ResFont("F_GothBallCrap.ttf"), 80.0f))
                .Add(UI::Label("Play", 0.5f), "Play")
                .Add(UI::Label("Leaderboard", 0.5f), "Leaderboard")
                .Add(UI::Label("Create", 0.5f), "Create")
                .Add(UI::Label("Edit", 0.5f), "Edit");
    
    UI = builder.Build(); 
}

void MenuMain::Update(double InDelta)
{
    CHECK_RETURN(!UI.IsValid())
    
    if (UI.Get("Play").IsClicked())
        Engine::Manager::Get().Push<GameInstance>();
    if (UI.Get("Create").IsClicked())
        Engine::Manager::Get().Push<RoomEditor>();
    if (UI.Get("Edit").IsClicked())
        Engine::Manager::Get().Push<BlueprintEditor>();
}
