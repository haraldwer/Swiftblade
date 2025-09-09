#include "LevelEntryWidget.h"

#include "UI/Builder.h"
#include "UI/Elements/Label.h"
#include "UI/Elements/List.h"
#include "UI/Elements/SplitContainer.h"

void UI::LevelEntryWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);

    transform.margins = { 5 };
    
    auto b = Builder()
        .Push(SplitContainer(Transform::Fill(), { SplitDirection::HORIZONTAL, 10, { 3, 1 }}))
            .Push(List(Transform::Fill(), { 0, ListDirection::HORIZONTAL }))
                .Add(Label(), "Name")
                .Add(Label({
                        .padding = {{ 5, 0}, {}},
                        .anchor = { 0, 0.95, },
                        .pivot = { 0, 1 }
                    }, {
                        .size= static_cast<float>(LabelSize::TEXT)
                    }), "Creator")
            .Pop()
            .Add(Label({}, { "Info" }), "Info")
            .Add(Label({}, { "*" }), "Star");
    Add(b.Build());

    Get<Label>("Name").SetText(entry.Name);
    Get<Label>("Creator").SetText(" by " + entry.Creator.Get());
    Get<Label>("Info").SetText(Utility::ToStr(entry.Plays));
    Get<Label>("Star").SetText(entry.Fav ? "*" : " ");
}

void UI::LevelEntryWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);

    float opacity = IsPressed() ? 0.5 : 0.0;
    background.color.a = Utility::Math::Lerp(background.color.a, opacity, 0.1f);
    if (IsHovered())
        background.color.a = Utility::Math::Max(background.color.a, 0.2f);

    if (IsClicked())
        InstanceEvent<LevelEntrySelected>::Invoke({ entry });
}

bool UI::LevelEntryWidget::IsHovered() const
{
    return Element::IsHovered();
}
