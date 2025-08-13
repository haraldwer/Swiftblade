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
        .Push(UI::List({ .position = {-600, 50}, .anchor = {1, 0}, .pivot = {0, 0}}, 20.0f, 0.0f, UI::List::FlowDirection::HORIZONTAL, true, 1.0));
    for (auto & option : std::ranges::reverse_view(options))
        builder.Add(UI::ButtonEditorTab(option), option);
    ui = builder.Build();
}

void MenuRoomEditor::Update()
{
    Instance::Update();

    if (selected.empty())
        selected = options.at(0);
    for (String& o : options)
        if (ui.Get<UI::ButtonEditorTab>(o).IsClicked())
            selected = o;
}

void MenuRoomEditor::SetSelected(const String &InStr)
{
    if (!ui.Contains(InStr))
    {
        LOG("Invalid option");
        selected = "";
    }
    for (String& o : options)
        ui.Get<UI::ButtonEditorTab>(o).SetSelected(o == selected);
}

