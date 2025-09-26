#pragma once
#include "Database/Events.h"
#include "UI/Widgets/LevelRoomList/LevelRoomListWidget.h"

namespace UI
{
    class InfoPanelRoom : public Container
    {
        CLASS_INFO(InfoPanelRoom, Container);
    public:
        InfoPanelRoom() : onInfo(this) {}
        InfoPanelRoom(const InfoPanelRoom& InOther) : Container(InOther), data(InOther.data), onInfo(this) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetRoom(const RoomEntryData& InData);

    private:
        void SetText(const String &InEntry, const String &InText);
        void SetEntryInfo(const RoomEntry& InEntry);
        void SetResourceInfo(const Room& InRoom);
        void RecieveInfo(const DB::Response<DB::RPCRoomInfo>& InResponse);

        RoomEntryData data;
        DB::Event<DB::RPCRoomInfo>::ContextCallback<InfoPanelRoom*> onInfo;
    };
}
