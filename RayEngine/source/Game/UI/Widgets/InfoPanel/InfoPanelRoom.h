#pragma once
#include "Editor/Room/RoomResource.h"
#include "UI/Elements/List.h"

namespace UI
{
    class InfoPanelRoom : public Container
    {
        TYPE_INFO(InfoPanelRoom, Container);
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetRoom(const ResRoom& InRoom);

    private:
        void SetText(const String &InEntry, const String &InText);
        ResRoom room;
    };
}
