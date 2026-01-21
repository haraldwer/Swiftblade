#include "LevelEntryWidget.h"

#include "GUID.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Elements/SplitContainer.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"

void UI::LevelEntryWidget::Init(Container &InOwner)
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
            .Pop()
            .Add(LabelText(), "Info")
            .Add(LabelHeader(), "Star");
    }
    
    Add(b.Build());
    UpdateInfo(data);
    
    onChanged.Bind([] (const auto& InData, auto InC)
    {
        InC->UpdateInfo(InData);
    });
}

void UI::LevelEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);

    float opacity = IsPressed() ? 0.5 : 0.0;
    background.color.a = Utility::Math::Lerp(background.color.a, opacity, 0.1f);
    if (IsHovered())
        background.color.a = Utility::Math::Max(background.color.a, 0.2f);

    if (IsClicked())
        InstanceEvent<LevelEntryData>::Invoke(data);
}

void UI::LevelEntryWidget::UpdateInfo(const LevelEntryData& InData)
{
    CHECK_RETURN(InData.add || data.add);

    if (InData.entry.ID.Get().empty())
    {
        CHECK_RETURN(!InData.resource.Identifier().IsValid());
        CHECK_RETURN(InData.resource != data.resource);
    }
    else
    {
        CHECK_RETURN(InData.entry.ID != data.entry.ID);
    }

    data = InData;

    String name = data.entry.Name;
    String creator = data.entry.CreatorName;
    String info = Utility::ToStr(data.entry.Score);
    bool starred = false;// data.entry.Fav.Get();

    if (auto res = data.resource.Get())
    {
        name = res->data.Name.Get();
        creator = res->data.Creator.Get();
    }

    if (name.empty())
        name = "Untitled";
    if (creator.empty())
        creator = "you";

    Get<Label>("Name").SetText(name);
    Get<Label>("Creator").SetText("by " + creator);
    Get<Label>("Info").SetText(info);
    Get<Label>("Star").SetText(starred ? "*" : "");
}
