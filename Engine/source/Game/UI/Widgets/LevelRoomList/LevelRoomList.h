#pragma once

#include "Level/RoomInfo.h"
#include "UI/Elements/List.h"
#include "UI/Menus/MenuRoomSelect.h"

class MenuRoomSelect;

namespace UI
{
    class LevelRoomList : public List
    {
        CLASS_INFO(LevelRoomList, List);
        
    public:
        LevelRoomList() : onSelect(this) {}
        LevelRoomList(const Transform& InTransform = Transform::Fill()) :
            List(InTransform, { .scrollable = true }, {}, true),
            onSelect(this) {}
        LevelRoomList(const LevelRoomList& InOther) :
            List(InOther),
            entries(InOther.entries),
            addButton(InOther.addButton),
            changed(InOther.changed),
            added(InOther.added),
            onSelect(InOther.onSelect, this),
            selectMenu(InOther.selectMenu) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetEntries(const Vector<RoomInfo>& InEntries);

        void AddEntryWidget(const RoomInfo& InData);

        Vector<RoomInfo> GetEntries() const { return entries; }
        bool IsChanged() const { return changed; }

    private:

        void SelectNew(const MenuRoomSelect::OnSelectEvent& InData);
        
        Vector<RoomInfo> entries;
        ElementID addButton = -1;
        bool changed = false;
        bool added = false;

        Engine::InstanceEvent<MenuRoomSelect::OnSelectEvent>::ContextCallback<LevelRoomList*> onSelect;
        MenuRoomSelect* selectMenu = nullptr;
    };
}
