#include "MenuMain.h"

#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Image.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"

void MenuMain::Init()
{
    // What a hierarchy might look like
    UI::Builder builder;
    builder.Push(UI::Container(UI::Transform::FromRect(600.0f, 20.0f)))
        .Push(UI::Container())
            .Add(UI::Image(ResTexture("godot+cpp.png")))
            .Push(UI::List(UI::Transform::Fill(10.0f, 15.0f), 0.0f, 10.0f));
    for (int i = 0; i < 6; i++)
        builder.Add(UI::Label("Test text", 0.5f));
    
    builder.Pop();
    UI = builder.Build();
}

void MenuMain::Update()
{
    UI.Update();
}

void MenuMain::Draw()
{
    UI.Draw();
}
