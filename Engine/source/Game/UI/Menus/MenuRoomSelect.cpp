#include "MenuRoomSelect.h"

#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelTitle.h"
#include "UI/Widgets/Common/Separator.h"
#include "UI/Widgets/RoomList/RoomListWidget.h"

void MenuRoomSelect::Init()
{
    UI::Builder builder = UI::Builder().
        Push(UI::Container(UI::Transform::Fill(0, 10), {{ 0, 0, 0, 0.5 }}))
            .Push(UI::List({
                .alignment = { 0, 1 },
                .anchor = { 0.5, 0 },
                .pivot = { 0.5, 0 },}))
            .Add(UI::LabelTitle({
                    .size = { 700, 50 }
                }, "Select room"))
            .Add(UI::Separator())
            .Add(UI::RoomListWidget("rooms_most_played", ""), "RoomList")
            .Push(UI::List(UI::Transform::Centered(),{ 5, UI::ListDirection::HORIZONTAL }))
                .Add(UI::ButtonDefault(UI::Transform::Centered(), { "Select" }), "Select")
                .Add(UI::ButtonDefault(UI::Transform::Centered(), { "Cancel" }), "Cancel");
    ui = builder.Build();
}

void MenuRoomSelect::Update()
{
    Instance::Update();
    if (ui["Select"].IsClicked())
        InstanceEvent<OnSelectEvent>::Invoke({ ui.Get<UI::RoomListWidget>("RoomList").Selected() });
    if (ui["Cancel"].IsClicked())
        InstanceEvent<OnSelectEvent>::Invoke({ {} });
}
