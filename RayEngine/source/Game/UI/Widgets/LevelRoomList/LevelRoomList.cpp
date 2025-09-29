#include "LevelRoomList.h"

#include "LevelRoomListWidget.h"
#include "Menu/Manager.h"
#include "UI/Widgets/Common/ButtonDefault.h"

void UI::LevelRoomList::Init(Container &InOwner)
{
    List::Init(InOwner);
    
    ButtonDefault b({}, "+");
    b.Init(*this);
    addButton = Add(b);

    onSelect.Bind([](const auto& InData, auto InC)
    {
        InC->SelectNew(InData);
    });
}

void UI::LevelRoomList::Update(Container &InOwner)
{
    List::Update(InOwner);
    changed = added;
    added = false;

    // All but add button
    for (int i = 0; i < Count() - 1; i++)
    {
        auto& w = Get<LevelRoomListWidget>(GetChild(i));
        if (w.IsClicked())
        {
            // Maybe show some details? 
        }
        if (w.IsRemoveClicked())
        {
            // Remove level
            entries.erase(entries.begin() + i);
            RemoveChild(i);
            i--;
            changed = true;
        }
    }

    if (Get<Element>(addButton).IsClicked() && !selectMenu)
        selectMenu = Menu::Manager::Get().Push<MenuRoomSelect>();
}

void UI::LevelRoomList::SetEntries(const Vector<RoomInfo> &InEntries)
{
    // All but add button
    while (Count() > 1)
        RemoveChild(0);
    
    entries = InEntries; 
    for (const auto& entry : entries)
        AddEntryWidget(entry);
}

void UI::LevelRoomList::AddEntryWidget(const RoomInfo& InData)
{
    LevelRoomListWidget widget(InData);
    widget.Init(*this);
    Insert(widget, Count() - 1);
}

void UI::LevelRoomList::SelectNew(const MenuRoomSelect::OnSelectEvent &InData)
{
    CHECK_RETURN(!selectMenu);
    
    if (!InData.entry.info.ID.Get().empty())
    {
        entries.push_back(InData.entry.info);
        AddEntryWidget(InData.entry.info);
    }
    
    Menu::Manager::Get().Close(selectMenu);
    selectMenu = nullptr;
    added = true;
}
