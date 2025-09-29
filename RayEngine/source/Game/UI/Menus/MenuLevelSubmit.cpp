#include "MenuLevelSubmit.h"

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

void MenuLevelSubmit::Init()
{
    UI::Builder b = UI::Builder()
        .Push(UI::TabContainer(UI::Transform::Fill(), {{0, 0, 0, 0.5}}), "Status")
            .Push(UI::List(UI::Transform::Centered(), {}, {}), "Info")
                .Add(UI::LabelTitle(UI::Transform::Centered(), "Submit level"))
                .Add(UI::Separator())
                .Add(UI::Spacing())
                .Add(UI::LabelText({}, "Once a level is submitted, it cannot be changed"))
                .Add(UI::Spacing())
                .Push(UI::List(UI::Transform::Centered(),{
                    .spacing = 5,
                    .direction = UI::ListDirection::HORIZONTAL
                }))
                    .Add(UI::ButtonDefault({}, "Submit"), "Submit")
                    .Add(UI::ButtonDefault({}, "Cancel"), "Cancel")
                .Pop()
            .Pop()
            .Push(UI::List(UI::Transform::Centered()), "Loading")
                .Add(UI::LoadingSpinner())
                .Add(UI::Spacing())
                .Add(UI::LabelHeader({}, "Submitting level..."))
            .Pop()
            .Push(UI::List(UI::Transform::Centered()), "Error")
                .Add(UI::LabelTitle(UI::Transform::Centered(), "Submission error"))
                .Add(UI::Separator())
                .Add(UI::Spacing())
                .Add(UI::LabelText({}, "Something went wrong when submitting."))
                .Add(UI::LabelText({}, ""), "ErrorMessage")
                .Add(UI::Spacing())
                .Push(UI::List(UI::Transform::Centered(), { .spacing = 5, .direction = UI::ListDirection::HORIZONTAL }))
                    .Add(UI::ButtonDefault({}, "Retry"), "Retry")
                    .Add(UI::ButtonDefault({}, "Close"), "Close")
                .Pop()
            .Pop()
            .Push(UI::List(UI::Transform::Centered()), "Submitted")
                .Add(UI::LabelTitle(UI::Transform::Centered(), "Submitted!"))
                .Add(UI::Separator())
                .Add(UI::Spacing())
                .Add(UI::LabelText({}, "Your level was submitted successfully\nand will now be visible online."))
                .Add(UI::Spacing())
                .Add(UI::ButtonDefault(UI::Transform::Centered(), "Close"), "Return")
            .Pop()
        .Pop();
    ui = b.Build();
    
    ui.Get<UI::TabContainer>("Status").Set("Info");
}

void MenuLevelSubmit::Update()
{
    Instance::Update();
    if (ui["Submit"].IsClicked() || ui["Retry"].IsClicked())
        InstanceEvent<OnClickedEvent>::Invoke({ "Submit" });
    if (ui["Return"].IsClicked() || ui["Close"].IsClicked() || ui["Cancel"].IsClicked())
        InstanceEvent<OnClickedEvent>::Invoke({ "Return" });
}

void MenuLevelSubmit::BeginLoading()
{
    auto& status = ui.Get<UI::TabContainer>("Status");
    status.SetVisible(true);
    status.Set("Loading");
}

void MenuLevelSubmit::SetSubmitState(bool InSuccess, const String &InError)
{
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
