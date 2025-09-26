#pragma once

#include "UI/Widgets/EditRoomList/EditRoomEntryWidget.h"

namespace UI
{
    class InfoPanelEditRoom : public Container
    {
        CLASS_INFO(InfoPanelEditRoom, Container);
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetRoom(const ResEditRoom& InRoom);

    private:
        void SetText(const String &InEntry, const String &InText);
        void SetResourceInfo(const EditRoom& InRoom);

        ResEditRoom room;
    };
}
