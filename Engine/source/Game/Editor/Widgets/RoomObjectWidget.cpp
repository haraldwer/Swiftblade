#include "RoomObjectWidget.h"

#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"

void UI::RoomObjectWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);
    Builder b = Builder()
        .Add(Image({.size{ 100 }}, { Icon }))
        .Add(Label({
                .position{0,-70},
                .anchor{0.5},
                .pivot{0.5}
            }, { Entry }));
    Add(b.Build());
}

void UI::RoomObjectWidget::Update(Container &InOwner)
{
    Container::Update(InOwner);

    // Do some kind of hover effect?
}
