#pragma once
#include "../LevelList/LevelEntryWidget.h"
#include "../EditRoomList/EditRoomEntryWidget.h"
#include "Instance/Events.h"
#include "UI/Elements/Container.h"
#include "Utility/EventScopes.h"

namespace UI
{
    class InfoPanel;
    typedef InstanceEvent<EditRoomEntryData>::ContextCallback<InfoPanel*> EditRoomCallback;
    typedef InstanceEvent<LevelEntryData>::ContextCallback<InfoPanel*> LevelCallback;

    class InfoPanel : public Container
    {
        CLASS_INFO(InfoPanel, Container);
    public:
        InfoPanel() :
            onEditRoomSelected(this),
            onLevelSelected(this) {}
        
        InfoPanel(const InfoPanel& InOther) :
            Container(InOther),
            onEditRoomSelected(InOther.onEditRoomSelected, this),
            onLevelSelected(InOther.onLevelSelected, this),
            onInstanceRemoved(InOther.onInstanceRemoved, this) {}
        
        void Init(Container& InOwner) override;
        void SetEditRoom(const EditRoomEntryData& InEvent);
        void SetLevel(const LevelEntryData& InEvent);
        void Clear();

    private:
        EditRoomCallback onEditRoomSelected;
        LevelCallback onLevelSelected;
        InstanceRemovedEvent::ContextCallback<InfoPanel*> onInstanceRemoved;
        bool pendingClear = false;
    };
}
