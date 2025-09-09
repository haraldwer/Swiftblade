#include "RoomEntryWidget.h"

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
            .Add(Image({ .size = { 0, 200 }}))
            .Add(Label({}, { .text = r.Name }));
    }
    else
    {
        builder
            .Add(Image({ .size = { 0, 200 }}))
            .Add(Label(Transform::Centered(), { .text= "+", .size= 100, .centering= 0.5 }));
    }
    
    Add(builder.Build());
}

void UI::RoomEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);
    if (IsClicked())
        InstanceEvent<RoomEntrySelected>::Invoke({ room });
}
