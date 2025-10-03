#pragma once
#include "BrowsePanel.h"
#include "Instance/Events.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"
#include "UI/Widgets/EditRoomList/EditRoomEntryWidget.h"

namespace UI
{
    class BrowseCreatePanel : public BrowsePanel
    {
        CLASS_INFO(BrowseCreatePanel, BrowsePanel);

    public:
        BrowseCreatePanel() :
            onAddedLevel(this),
            onAddedRoom(this),
            onInstanceRemoved(this){}
        BrowseCreatePanel(const BrowseCreatePanel& InOther) :
            BrowsePanel(InOther),
            onAddedLevel(InOther.onAddedLevel, this),
            onAddedRoom(InOther.onAddedRoom, this),
            onInstanceRemoved(InOther.onInstanceRemoved, this) {}
        BrowseCreatePanel(LevelEntryWidget&& InOther) = delete;
        
        void Init(Container& InOwner) override;
        void Update(Container& InOwner) override;

    private:

        void NewLevel(const LevelEntryData& InData);
        void NewRoom(const EditRoomEntryData& InData);
        
        void SelectLevels();
        void SelectRooms();

        InstanceEvent<LevelEntryData>::ContextCallback<BrowseCreatePanel*> onAddedLevel;
        InstanceEvent<EditRoomEntryData>::ContextCallback<BrowseCreatePanel*> onAddedRoom;
        InstanceRemovedCallbackT<BrowseCreatePanel*> onInstanceRemoved;

        bool pendingRefresh = false;
        bool levelsSelected = false;
    };
}
