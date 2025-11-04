#include "RoomObjectWidget.h"

#include "UI/Builder.h"
#include "UI/Elements/Image.h"
#include "UI/Elements/Label.h"

void UI::RoomObjectWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);
    Builder b = Builder()
        .Add(Image(Transform::Fill(), { Icon }))
        .Add(Label({{0,-50}}, { Entry }));
    Add(b.Build());
}
