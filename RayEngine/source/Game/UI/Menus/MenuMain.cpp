#include "MenuMain.h"

#include "MenuLeaderboard.h"
#include "Editor/BlueprintEditor.h"
#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Image.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Elements/RectImage.h"
#include "game/GameInstance.h"
#include "Game/Editor/RoomEditor.h"

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

    const UI::Builder buttonWidget = UI::Builder()
       .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
       .Add(UI::Label("Text", 0.5f), "Text");
    for (const String& b : buttons)
        builder.AddWidget(buttonWidget, UI::Transform::Fill(), b);
    
    UI = builder.Build();

    for (const String& b : buttons)
        UI->Get<UI::Instance>(b).Get<UI::Label>("Text").SetText(b);
}

void MenuMain::Update(double InDelta)
{
    CHECK_RETURN(!UI)
    
    if (UI->Get("Play").IsClicked())
        Engine::Manager::Get().Push<GameInstance>();
    if (UI->Get("Leaderboard").IsClicked())
        Menu::Manager::Get().Push<MenuLeaderboard>();
    if (UI->Get("Create").IsClicked())
        Engine::Manager::Get().Push<RoomEditor>();
    if (UI->Get("Edit").IsClicked())
        Engine::Manager::Get().Push<BlueprintEditor>(); 
}
