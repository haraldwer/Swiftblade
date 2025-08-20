#include "MenuSettings.h"

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void MenuSettings::Init()
{
    auto b = UI::Builder()
        .Push(UI::List(UI::Transform::Centered()))
            .Add(UI::Label({}, "Settings", 50))
            .Add(UI::Label({}, "Back"), "Back");

    ui = b.Build(); 
}

void MenuSettings::Update()
{
    Instance::Update();

    if (ui["Back"].IsClicked())
        Menu::Manager::Get().Pop();
}
