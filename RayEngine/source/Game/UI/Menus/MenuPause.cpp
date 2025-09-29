
#include "MenuPause.h"

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "Input/Action.h"
#include "Instance/Manager.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelTitle.h"

void MenuPause::Init()
{
    UI::Builder builder = UI::Builder()
        .Push(UI::List(UI::Transform::Centered(), { 10 }))
            .Add(UI::LabelTitle(UI::Transform::Centered(), "PAUSED"))
            .Add(UI::LabelHeader(UI::Transform::Centered(), "Resume"), "Resume")
            .Add(UI::LabelHeader(UI::Transform::Centered(), "Quit"), "Quit");

    ui = builder.Build();
    
    Utility::Time::Get().SetPause(true);
    Input::Manager::Get().Push("Default");
}

void MenuPause::Update()
{
    Instance::Update();

    if (Input::Action::Get("Back").Pressed() ||
        ui["Resume"].IsClicked())
        Menu::Manager::Get().Close(this);

    if (ui["Quit"].IsClicked())
        Engine::Manager::Get().Pop();
}

void MenuPause::Deinit()
{
    Instance::Deinit();
    Input::Manager::Get().Pop("Default");
    Utility::Time::Get().SetPause(false);
}
