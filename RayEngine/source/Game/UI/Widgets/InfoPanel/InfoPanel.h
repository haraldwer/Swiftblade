#pragma once
#include "../LevelList/LevelEntryWidget.h"
#include "../EditRoomList/EditRoomEntryWidget.h"
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
        InfoPanel()
        {
            onEditRoomSelected.SetContext(this);
            onLevelSelected.SetContext(this);
        }
        
        InfoPanel(const InfoPanel& InOther) :
            Container(InOther),
            onEditRoomSelected(InOther.onEditRoomSelected),
            onLevelSelected(InOther.onLevelSelected)
        {
            onEditRoomSelected.SetContext(this);
            onLevelSelected.SetContext(this);
        }
        
        void Init(Container& InOwner) override;
        void SetEditRoom(const EditRoomEntryData& InEvent);
        void SetLevel(const LevelEntryData& InEvent);

    private:
        EditRoomCallback onEditRoomSelected;
        LevelCallback onLevelSelected;
    };
}
