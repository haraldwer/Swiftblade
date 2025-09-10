#include "MenuRoomPreview.h"

#include "UI/Builder.h"
#include "UI/Elements/List.h"
#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelText.h"
#include "UI/Widgets/Common/LabelTitle.h"

void MenuRoomPreview::Init()
{
    UI::Builder b = UI::Builder()
        .Push(UI::List({
                .position = { -50, 0 },
                .margins = 10,
                .anchor = { 1, 0.5 },
                .pivot = { 1, 0.5 },
            }, {},
            {
                .color = { 1, 1, 1, 0.5 }
            }))
            .Add(UI::LabelTitle({}, "Submit room"))
            .Add(UI::LabelText({}, "Once a room is submitted, it cannot be changed"))
            .Add(UI::ButtonDefault({}, "Submit"), "Submit");

    ui = b.Build();
}

void MenuRoomPreview::Update()
{
    Instance::Update();

    if (ui["Submit"].IsClicked())
    {
        LOG("TODO: Submit room");

        
    }
}
