#pragma once
#include "UI/Widgets/RoomList/RoomEntryWidget.h"

namespace UI
{
    class InfoPanelRoom : public Container
    {
        CLASS_INFO(InfoPanelRoom, Container);
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetRoom(const RoomEntryData& InData);

    private:
        void SetText(const String &InEntry, const String &InText);

        RoomEntryData data;
    };
}
