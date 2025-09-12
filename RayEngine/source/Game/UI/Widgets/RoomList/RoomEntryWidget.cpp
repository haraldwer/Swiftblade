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

    Builder builder = Builder();
    
    if (room.Identifier().IsValid())
    {
        LOG("Found room: " + Utility::ReadFile(room.Identifier().Str()));
        auto res = room.Get();
        auto r = res->Get();
        builder
            .Add(Image())
            .Add(LabelHeader({
                .padding = { 10 },
                .anchor = { 0, 1 }, 
                .pivot = { 0, 1 },
            }, r.Name));
    }
    else
    {
        builder
            .Add(Image())
            .Add(Label(Transform::Centered(), {
                .text= "+",
                .size= 100
            }));
    }
    
    Add(builder.Build());
    SetBackground({ { 1, 1, 1, 0.5 } });
    SetTransform({
        .size = { 0, 200 },
        .alignment = { 1, 1 },
        .padding = { 0, 5 }
    });
}

void UI::RoomEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);
    if (IsClicked())
        InstanceEvent<RoomEntrySelected>::Invoke({ room });
}
