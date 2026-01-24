#include "MenuRoomPreview.h"

#include "Editor/RoomEditor.h"
#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Elements/TabContainer.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/LabelTitle.h"
#include "UI/Widgets/Common/LoadingSpinner.h"
#include "UI/Widgets/Common/Separator.h"
#include "UI/Widgets/Common/Spacing.h"

void MenuRoomPreview::Init()
{
    UI::Builder b = UI::Builder()
        .Push(UI::List({
                .position = { -50, -50 },
                .margins = 10,
                .anchor = { 1, 1 },
                .pivot = { 1, 1 },
            }, {},
            {
                .color = { 1, 1, 1, 0.5 }
            }), "Info")
            .Add(UI::LabelTitle({}, "Submit room"))
            .Add(UI::LabelText({}, "Once a room is submitted, it cannot be changed"))
            .Add(UI::Spacing())
            .Push(UI::List({},{
                .spacing = 5,
                .direction = UI::ListDirection::HORIZONTAL
            }))
                .Add(UI::ButtonDefault({}, { "Submit" }), "Submit")
                .Add(UI::ButtonDefault({}, { "Preview" }), "Preview")
            .Pop()
        .Pop()
        .Push(UI::TabContainer(UI::Transform::Fill(), {{0, 0, 0, 0.5}}), "Status")
            .Push(UI::List(UI::Transform::Centered()), "Loading")
                .Add(UI::LoadingSpinner())
                .Add(UI::Spacing())
                .Add(UI::LabelHeader({}, "Submitting room..."))
            .Pop()
            .Push(UI::List(UI::Transform::Centered()), "Error")
                .Add(UI::LabelTitle({}, "Submission error"))
                .Add(UI::Separator())
                .Add(UI::Spacing())
                .Add(UI::LabelText({}, "Something went wrong when submitting."))
                .Add(UI::LabelText({}, ""), "ErrorMessage")
                .Add(UI::Spacing())
                .Push(UI::List(UI::Transform::Centered(), { .spacing = 5, .direction = UI::ListDirection::HORIZONTAL }))
                    .Add(UI::ButtonDefault({}, { "Retry" }), "Retry")
                    .Add(UI::ButtonDefault({}, { "Close" }), "Close")
                .Pop()
            .Pop()
            .Push(UI::List(UI::Transform::Centered()), "Submitted")
                .Add(UI::LabelTitle(UI::Transform::Centered(), "Submitted!"))
                .Add(UI::Separator())
                .Add(UI::Spacing())
                .Add(UI::LabelText({}, "Your room was submitted successfully\nand will now be visible online."))
                .Add(UI::Spacing())
                .Add(UI::ButtonDefault(UI::Transform::Centered(), { "Return to menu" }), "Return")
            .Pop()
        .Pop();
        

    ui = b.Build();
    ui["Status"].SetVisible(false);
}

void MenuRoomPreview::Update()
{
    Instance::Update();
    if (ui["Submit"].IsClicked() || ui["Retry"].IsClicked())
        Engine::InstanceEvent<OnClickedEvent>::Invoke({ "Submit" });
    if (ui["Preview"].IsClicked())
        Engine::InstanceEvent<OnClickedEvent>::Invoke({ "Preview" });
    if (ui["Return"].IsClicked())
        Engine::InstanceEvent<OnClickedEvent>::Invoke({ "Return" });
    if (ui["Close"].IsClicked())
        ui["Status"].SetVisible(false);
}

void MenuRoomPreview::BeginLoading()
{
    auto& status = ui.Get<UI::TabContainer>("Status");
    status.SetVisible(true);
    status.Set("Loading");
}

void MenuRoomPreview::SetSubmitState(bool InSuccess, const String &InError)
{
    ui["Status"].SetVisible(true);

    if (InSuccess)
    {
        ui.Get<UI::TabContainer>("Status").Set("Submitted");
    }
    else
    {
        ui.Get<UI::TabContainer>("Status").Set("Error");
        ui.Get<UI::Label>("ErrorMessage").SetText(InError);
    }
}
