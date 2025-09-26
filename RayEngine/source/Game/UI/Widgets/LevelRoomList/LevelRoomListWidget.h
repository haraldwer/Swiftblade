#pragma once

#include "Database/Data/RPCRoom.h"
#include "Level/Room.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct RoomEntryData
    {
        RoomEntry entry;
        ResRoom resource;
        bool add;
    };
    
    class LevelRoomListWidget : public Container
    {
    public:
        LevelRoomListWidget(const RoomEntryData& InData) : Container({ .alignment = {1, 0} }), data(InData) {}
        void Init(Container &InOwner) override;
        String GetID() const { return data.entry.ID; }

        bool RemoveClicked();
        
    private:
        void SetInfo(const Room& InRoom);
        
        RoomEntryData data;
        ElementID removeButton = -1;
        ElementID label = -1;
    };
}
