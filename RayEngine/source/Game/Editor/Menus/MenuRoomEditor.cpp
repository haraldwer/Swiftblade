#include "MenuRoomEditor.h"

#include <ranges>

#include "Editor/Widgets/ButtonEditorTab.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"

void MenuRoomEditor::Init()
{
    UI::Builder builder = UI::Builder()
        .Push(UI::List(
            {
                .position = {-600, 50},
                .anchor = {1, 0},
                .pivot = {0, 0}
            },
            20.0f,
            0.0f,
            UI::List::FlowDirection::HORIZONTAL,
            true,
            1.0),
        "List");
    for (auto & option : std::ranges::reverse_view(options))
        builder.Add(UI::ButtonEditorTab(option), option);
    ui = builder.Build();
}

void MenuRoomEditor::Update()
{
    Instance::Update();

    auto& l = ui.Get<UI::List>("List");
    for (String& o : options)
    {
        auto& b = l.Get<UI::ButtonEditorTab>(o);
        b.SetSelected(o == selected);
        if (b.IsClicked())
            OnClicked.Invoke({ o });
        
    }
}

void MenuRoomEditor::SetSelected(const String &InStr)
{
    selected = InStr;
}

