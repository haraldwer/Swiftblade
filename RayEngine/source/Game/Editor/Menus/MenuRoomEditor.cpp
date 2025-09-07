#include "MenuRoomEditor.h"

#include <ranges>

#include "../../UI/Widgets/TabButton.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void MenuRoomEditor::Init()
{
    UI::Builder builder = UI::Builder()
        .Push(UI::List(
            {
                .position = {-610, 50},
                .anchor = {1, 0},
                .pivot = {0, 0}
            },
            20.0f,
            0.0f,
            UI::List::FlowDirection::HORIZONTAL,
            true,
            1.0),
        "List");
    
    for (auto& o : std::ranges::reverse_view(options))
        builder.Add(UI::TabButton(o.text), o.name);
    
    ui = builder.Build();
}

void MenuRoomEditor::Update()
{
    Instance::Update();

    auto& l = ui.Get<UI::List>("List");
    for (Option& o : options)
    {
        auto& b = l.Get<UI::TabButton>(o.name);
        b.SetSelected(o.name == selected);
        if (b.IsClicked())
            InstanceEvent<OnClickedEvent>::Invoke({ o.name });
        
    }
}

void MenuRoomEditor::AddOption(const String &InName, const String &InText)
{
    options.push_back({ InName, InText });
}

void MenuRoomEditor::SetSelected(const String &InStr)
{
    selected = InStr;
}

