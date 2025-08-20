#include "InfoPanel.h"

#include "Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void UI::InfoPanel::Init(Container &InOwner)
{
    Container::Init(InOwner);
    
    auto b = Builder()
        .Push(List({ .margins = 10 }))
            .Add(Label({}, "InfoPanel"), "Title")
            .Push(Container(), "Content")
                .Add(Label({}, "Testing!"), "Text");
    
    root = Add(b.Build());

    SetBackground({
        .color = { 1, 1, 1, 0.5 }
    });
}

void UI::InfoPanel::SetRoom(String InObj)
{
    Engine::Manager::Get().Push<GameInstance>();
}

void UI::InfoPanel::SetLevel(String InObj)
{
    Engine::Manager::Get().Push<GameInstance>();
}

void UI::InfoPanel::Populate()
{
    
}
