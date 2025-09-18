#include "LevelEntryWidget.h"

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Elements/SplitContainer.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/LabelTitle.h"

void UI::LevelEntryWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);

    transform.margins = { 0 };
    
    auto b = Builder()
        .Push(TabContainer(Transform::Fill(), {}), "Switch")
            .Push(SplitContainer(Transform::Fill(5), { 10, SplitDirection::HORIZONTAL, { 3, 1 }}), "Data")
                .Push(List(Transform::Fill(), { 0, ListDirection::HORIZONTAL }))
                    .Add(LabelHeader(), "Name")
                    .Add(LabelText({
                            .padding = {{ 5, 0}, {}},
                            .anchor = { 0, 0.95, },
                            .pivot = { 0, 1 }
                        }), "Creator")
                .Pop()
                .Add(LabelText({}, { "Info" }), "Info")
                .Add(LabelHeader({}, { "*" }), "Star")
            .Pop()
            .Add(LabelHeader(Transform::Fill(), { "+" }, {{ 1,1,1,0.5 }}), "Add");
    
    Add(b.Build());
    
    RefreshInfo();
}

void UI::LevelEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);

    float opacity = IsPressed() ? 0.5 : 0.0;
    background.color.a = Utility::Math::Lerp(background.color.a, opacity, 0.1f);
    if (IsHovered())
        background.color.a = Utility::Math::Max(background.color.a, 0.2f);

    if (IsClicked())
        InstanceEvent<LevelEntrySelected>::Invoke({ listEntry, levelResource, add });
}

bool UI::LevelEntryWidget::IsHovered() const
{
    return Element::IsHovered();
}

void UI::LevelEntryWidget::RefreshInfo()
{
    if (add)
    {
        Get<TabContainer>("Switch").Set("Add");
    }
    else
    {
        Get<TabContainer>("Switch").Set("Data");
        
        Get<Label>("Name").SetText(listEntry.Name);
        Get<Label>("Creator").SetText(" by " + listEntry.Creator.Get());
        Get<Label>("Info").SetText(Utility::ToStr(listEntry.Plays));
        Get<Label>("Star").SetText(listEntry.Fav ? "*" : " ");

        if (auto res = levelResource.Get())
        {
            auto& data = res->data;
            
            Get<Label>("Name").SetText(data.Name);
            Get<Label>("Creator").SetText(" by " + data.Creator.Get());
            Get<Label>("Info").SetText(data.LastEdit);
            Get<Label>("Star").SetText("");
        }
    }
}
