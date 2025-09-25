#pragma once
#include "../LevelList/LevelEntryWidget.h"
#include "../RoomList/RoomEntryWidget.h"
#include "UI/Elements/Container.h"
#include "Utility/EventScopes.h"

namespace UI
{
    class InfoPanel;
    typedef InstanceEvent<RoomEntryData>::ContextCallback<InfoPanel*> RoomCallback;
    typedef InstanceEvent<LevelEntryData>::ContextCallback<InfoPanel*> LevelCallback;

    class InfoPanel : public Container
    {
        CLASS_INFO(InfoPanel, Container);
    public:
        InfoPanel()
        {
            onRoomSelected.SetContext(this);
            onLevelSelected.SetContext(this);
        }
        
        InfoPanel(const InfoPanel& InOther) :
            Container(InOther),
            onRoomSelected(InOther.onRoomSelected),
            onLevelSelected(InOther.onLevelSelected)
        {
            onRoomSelected.SetContext(this);
            onLevelSelected.SetContext(this);
        }
        
        void Init(Container& InOwner) override;
        void SetRoom(const RoomEntryData& InEvent);
        void SetLevel(const LevelEntryData& InEvent);

    private:
        RoomCallback onRoomSelected;
        LevelCallback onLevelSelected;
    };
}
