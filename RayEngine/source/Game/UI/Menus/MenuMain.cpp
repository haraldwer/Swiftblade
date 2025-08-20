#include "MenuMain.h"

#include "MenuBrowse.h"
#include "Editor/RoomEditor.h"
#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/Image.h"
#include "Engine/UI/Elements/Label.h"
#include "Engine/UI/Elements/List.h"
#include "MenuProfile.h"
#include "MenuSettings.h"
#include "UI/Widgets/BrowseCommunityPanel.h"
#include "UI/Widgets/BrowseCreatePanel.h"
#include "UI/Widgets/BrowseStoryPanel.h"

void MenuMain::Init()
{
    UI::Builder b = UI::Builder()
        .Push(UI::Container(UI::Transform::Fill()))
            .Push(UI::List({
                    .position = {0, 0},
                    .alignment = { { 0, 1 }, { 0, 0 } },
                    .padding = 30, 
                    .anchor = {1.0, 0},
                    .pivot = 0,
                }, 10, 0, UI::List::FlowDirection::HORIZONTAL, true))
                .Add(UI::Image({ .size = 40 }), "Exit")
                .Add(UI::Image({ .size = 40 }), "Settings")
                .Add(UI::Image({ .size = 40 }), "Profile")
            .Pop()
            .Push(UI::List(UI::Transform::Centered(), 0, 0, UI::List::FlowDirection::VERTICAL, false, 1))
                .Add(UI::Label({}, "Swiftblade", 150, ResFont("UI/F_GothBallCrap.ttf")))
                .Push(UI::List(UI::Transform::Centered(), 50, 0,  UI::List::FlowDirection::HORIZONTAL, false, 1), "List");
    
    const Vector<String> buttons = {
        "Play",
        "Browse",
        "Create"
    };

    UI::Container buttonWidget = UI::Builder()  
        .Add(UI::Label({}, "Text", 50, ResFont("UI/F_LinLibertine_aSZI.ttf")), "Text")
        .Build();
    
    for (const String& button : buttons)
    {
        buttonWidget.Get<UI::Label>("Text").SetText(button);
        b.Add(buttonWidget, button);
    }
    
    ui = b.Build();
}

void MenuMain::Update()
{
    if (ui["Play"].IsClicked())
        if (auto browse = Menu::Manager::Get().Push<MenuBrowse>())
            browse->SetPanelType(Type::Get<UI::BrowseStoryPanel>());
    if (ui["Browse"].IsClicked())
        if (auto browse = Menu::Manager::Get().Push<MenuBrowse>())
            browse->SetPanelType(Type::Get<UI::BrowseCommunityPanel>());
    if (ui["Create"].IsClicked())
        if (auto browse = Menu::Manager::Get().Push<MenuBrowse>())
            browse->SetPanelType(Type::Get<UI::BrowseCreatePanel>());
    if (ui["Profile"].IsClicked())
        Menu::Manager::Get().Push<MenuProfile>();
    if (ui["Settings"].IsClicked())
        Menu::Manager::Get().Push<MenuSettings>();
    if (ui["Exit"].IsClicked())
        Engine::Manager::Get().Pop();
}
