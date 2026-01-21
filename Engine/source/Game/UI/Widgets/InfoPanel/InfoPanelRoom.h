#pragma once
#include "Database/Events.h"
#include "Database/Data/RPCRoom.h"
#include "UI/Widgets/LevelRoomList/LevelRoomListWidget.h"

namespace UI
{
    class InfoPanelRoom : public Container
    {
        CLASS_INFO(InfoPanelRoom, Container);
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetRoom(const ResRoom& InResource);

    private:
        void SetText(const String &InEntry, const String &InText);

        ResRoom resource;
    };
}
