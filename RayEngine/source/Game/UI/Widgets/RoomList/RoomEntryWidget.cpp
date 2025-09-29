#include "RoomEntryWidget.h"

#include "GUID.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Elements/SplitContainer.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"

void UI::RoomEntryWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);

    transform.margins = { 0 };
    
    auto b = Builder();

    if (data.add)
    {
        b.Add(LabelHeader(Transform::Fill(), { "+" }, {{ 1,1,1,0.5 }}));
    }
    else
    {
        b.Push(SplitContainer(Transform::Fill(5), { 10, SplitDirection::HORIZONTAL, { 3, 1 }}))
            .Push(List(Transform::Fill(), { 0, ListDirection::HORIZONTAL }))
                .Add(LabelHeader(), "Name")
                .Add(LabelText({
                        .padding = {{ 5, 0}, {}},
                        .anchor = { 0, 0.95, },
                        .pivot = { 0, 1 }
                    }, {}), "Creator")
            .Pop();
    }
    
    Add(b.Build());
    UpdateInfo(data);
    
    onChanged.Bind([] (const auto& InData, auto InC)
    {
        InC->UpdateInfo(InData);
    });
}

void UI::RoomEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);

    float opacity = IsPressed() ? 0.5 : 0.0;
    background.color.a = Utility::Math::Lerp(background.color.a, opacity, 0.1f);
    if (IsHovered())
        background.color.a = Utility::Math::Max(background.color.a, 0.2f);
}

void UI::RoomEntryWidget::UpdateInfo(const RoomEntryData& InData)
{
    CHECK_RETURN(InData.add || data.add);
    
    CHECK_RETURN(InData.info.ID.Get().empty())
    CHECK_RETURN(InData.info.ID != data.info.ID);

    data = InData;

    String name = data.info.Name;
    String creator = data.info.CreatorName;

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";

    Get<Label>("Name").SetText(name);
    Get<Label>("Creator").SetText("by " + creator);
}
