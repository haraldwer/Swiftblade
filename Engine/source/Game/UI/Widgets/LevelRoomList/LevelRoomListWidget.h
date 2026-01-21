#pragma once

#include "Level/RoomInfo.h"
#include "UI/Elements/Container.h"

namespace UI
{
    class LevelRoomListWidget : public Container
    {
        CLASS_INFO(LevelRoomListWidget, Container);
    public:
        LevelRoomListWidget(const RoomInfo& InData) : Container({ .alignment = {1, 0} }), data(InData) {}
        void Init(Container &InOwner) override;
        String GetID() const { return data.ID.Get(); }

        bool IsRemoveClicked();
        
        RoomInfo data;
        ElementID removeButton = -1;
    };
}
