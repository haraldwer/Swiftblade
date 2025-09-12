#pragma once
#include "Database/Data/RPCLevelList.h"
#include "Editor/Room/RoomResource.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct RoomEntrySelected
    {
        ResRoom room;
    };
    
    class RoomEntryWidget : public Container
    {
        CLASS_INFO(RoomEntryWidget, Container);
    public:
        RoomEntryWidget() = default;
        RoomEntryWidget(const ResRoom& InRoom) : Container(), room(InRoom) {}
        RoomEntryWidget(const DB::RPCRoomList::Entry& InRoom) : Container(), roomEntry(InRoom) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

    private:
        ResRoom room;
        DB::RPCRoomList::Entry roomEntry;
    };
}
