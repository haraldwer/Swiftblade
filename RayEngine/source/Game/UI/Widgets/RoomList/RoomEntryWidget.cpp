#include "RoomEntryWidget.h"

#include "../Common/LabelHeader.h"
#include "Editor/RoomEditor.h"
#include "Instance/Manager.h"
#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"

void UI::RoomEntryWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder b = Builder();
    
    if (data.add)
    {
        b.Add(Label(Transform::Centered(), {
               .text= "+",
               .size= 100
           }));
    }
    else
    {
        b.Add(Image(Transform::Fill()), "Thumb");
        b.Add(LabelHeader({
                .padding = { 10 },
                .anchor = { 0, 1 }, 
                .pivot = { 0, 1 },
            }), "Name");
    }
    
    Add(b.Build());
    SetBackground({ { 1, 1, 1, 0.5 } });
    SetTransform({
        .size = { 0, 200 },
        .alignment = { 1, 1 },
        .padding = { 0, 5 }
    });

    UpdateInfo(data);

    onChanged.Bind([](const auto& InData, auto InC)
    {
        InC->UpdateInfo(InData);
    });
}

void UI::RoomEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);
    if (IsClicked())
        InstanceEvent<RoomEntryData>::Invoke(data);
}

void UI::RoomEntryWidget::UpdateInfo(const RoomEntryData &InData)
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
    String thumbPath = "";
    
    if (auto res = data.resource.Get())
    {
        if (!res->data.Name.Get().empty() && name.empty())
            name = res->data.Name;
        thumbPath = data.resource.Identifier().Str() + ".png";
            
    }

    if (name.empty())
        name = "Untitled";

    Get<Label>("Name").SetText(name);
    Get<Image>("Thumb").SetProperties({ thumbPath });
}
