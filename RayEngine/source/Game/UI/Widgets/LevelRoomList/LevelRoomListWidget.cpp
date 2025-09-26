#include "LevelRoomListWidget.h"

#include "UI/Widgets/Common/ButtonDefault.h"
#include "UI/Widgets/Common/LabelText.h"

void UI::LevelRoomListWidget::Init(Container &InOwner)
{
    Container::Init(InOwner);

    LabelText l;
    l.Init(*this);
    label = Add(l);

    ButtonDefault b({
            .size = 50,
            .anchor = { 1, 0.5 },
            .pivot = { 1, 0.5 } },
        "-");
    b.Init(*this);
    removeButton = Add(b);
    
    String cache = Utility::GetCachePath(data.entry.ID, ".json");
    if (Utility::FileExists(cache))
    {
        ResRoom room = cache;
        if (auto res = room.Get())
        {
            SetInfo(res->data);
            return;
        }
    }

    // Request from server!
    LOG("Could not find room cache");
}

bool UI::LevelRoomListWidget::RemoveClicked()
{
    return Get<Element>(removeButton).IsClicked();
}

void UI::LevelRoomListWidget::SetInfo(const Room& InRoom)
{
    Get<Label>(label).SetText(InRoom.Name.Get());
}
