#include "LevelRoomList.h"

#include "LevelRoomListWidget.h"

void UI::LevelRoomList::Init(Container &InOwner)
{
    List::Init(InOwner);
    
    // + button
}

void UI::LevelRoomList::SetEntries(const Vector<RoomEntry> &InEntries)
{
    ClearChildren();
    entries = InEntries;
    for (const auto& entry : entries)
    {
        RoomEntryData data;
        data.entry = entry;
        LevelRoomListWidget widget(data);
        widget.Init(*this);
        Insert(widget, Count() - 1);
    }
}
