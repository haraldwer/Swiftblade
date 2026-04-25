#include "MenuSettings.h"

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelTitle.h"

void MenuSettings::Init()
{
    auto b = UI::Builder()
        .Push(UI::List(UI::Transform::Centered()))
            .Add(UI::LabelTitle({}, "Settings"))
            .Add(UI::LabelHeader({}, "Back"), "Back");

    ui = b.Build(); 
}

void MenuSettings::Update()
{
    Instance::Update();

    if (ui["Back"].IsClicked())
        Menu::Manager::Get().Close(this);
}
