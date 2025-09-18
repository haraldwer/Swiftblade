#include "InfoPanelLevel.h"

#include "Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/TextboxDefault.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"

void UI::InfoPanelLevel::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder(Container({
                .alignment = 1,
                .margins = 10
            }))
        .Push(List())
            .Push(TabContainer(), "NameTab")
                .Push(List({}, { 10, ListDirection::HORIZONTAL}), "NameShow")
                    .Add(ButtonDefault({ .anchor = 0.5, .pivot = 0.5 }, " "), "EditName")
                    .Push(List({ .anchor = { 0, 0.5 }, .pivot = { 0, 0.5 }}))
                        .Add(LabelHeader(), "Name")
                        .Add(LabelText({.padding = {0, {-5, 0}}}), "Creator")
                    .Pop()
                .Pop()
                .Push(TextboxDefault({
                        .alignment = { 1, 1 }
                    }, {
                        .maxChars = 25,
                        .centering = { 0, 0.5 },
                    }), "NameEdit")
            .Pop()
            .Add(LabelHeader(), "Rooms")
            .Push(List(Transform::Fill(), {}, {{ 0, 0, 0, 0.5 }}), "RoomList")
                // Add rooms here    
            .Pop()
            .Add(LabelHeader(), "Arenas")
            .Push(List(Transform::Fill(), {}, {{ 0, 0, 0, 0.5 }}), "ArenaList")
                // Add arenas here
            .Pop()
        .Pop()
        .Push(List({
                .position = {},
                .anchor = {1, 1},
                .pivot = {1, 1}
            },  {
                .direction = ListDirection::HORIZONTAL
            }))
            .Add(ButtonDefault({}, "Play"), "Play");
    Add(b.Build());

    Get<TabContainer>("NameTab").Set("NameShow");
}

void UI::InfoPanelLevel::Update(Container &InOwner)
{
    Container::Update(InOwner);

    if (Get<ButtonDefault>("EditName").IsClicked())
        Get<TabContainer>("NameTab").Set("NameEdit");

    auto& tbx = Get<Textbox>("NameEdit");
    if (tbx.IsCommitted())
    {
        String text = tbx.GetText();
        if (text.empty())
            text = "Untitled";
        
        // Apply new name!
        Get<Label>("Name").SetText(text);
        Get<TabContainer>("NameTab").Set("NameShow");
    }
    
    if (Get<ButtonDefault>("Play").IsClicked())
    {
        if (auto game = Engine::Manager::Get().Push<GameInstance>())
        {
            // Set level!
        }
    }
}

void UI::InfoPanelLevel::SetLevel(const LevelEntrySelected &InLevel)
{
    Get<Label>("Name").SetText(InLevel.entry.Name);
    Get<Label>("Creator").SetText(InLevel.entry.Creator);
    //Get<List>("RoomList").ClearChildren();
    //Get<List>("ArenaList").ClearChildren();

    if (auto res = InLevel.level.Get())
    {
        Get<Label>("Name").SetText(res->data.Name);
        Get<Label>("Creator").SetText(res->data.Creator);
    }
    else
    {
        // At this point maybe fetch level from server?
    }
    
    if (InLevel.add)
    {
        
    }

    Get<Textbox>("NameEdit").SetText(Get<Label>("Name").GetText());
}
