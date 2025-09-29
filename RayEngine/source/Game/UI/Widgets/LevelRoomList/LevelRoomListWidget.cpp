#include "LevelRoomListWidget.h"

#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelText.h"

void UI::LevelRoomListWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);

    Builder builder = Builder().
        Push(List(Transform::Fill(), { 5, ListDirection::HORIZONTAL }))
            .Add(LabelText({.anchor{0, 0.5}, .pivot{0, 0.5} }, data.Name))
            .Add(LabelText({.anchor{0, 0.5}, .pivot{0, 0.5} }, "by " + data.CreatorName.Get()));
    Add(builder.Build());
    
    ButtonDefault b({
            .size = 20,
            .anchor = { 1, 0.5 },
            .pivot = { 1, 0.5 } },
        "-");
    b.Init(*this);
    removeButton = Add(b);
}

bool UI::LevelRoomListWidget::IsRemoveClicked()
{
    return Get<Element>(removeButton).IsClicked();
}