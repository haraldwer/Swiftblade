#pragma once
#include "Editor/Room/RoomResource.h"
#include "UI/Elements/List.h"

namespace UI
{
    struct RoomEntrySelected;
    
    class InfoPanelRoom : public Container
    {
        CLASS_INFO(InfoPanelRoom, Container);
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetRoom(const RoomEntrySelected& InRoom);

    private:
        void SetText(const String &InEntry, const String &InText);
        ResRoom room;
    };
}
