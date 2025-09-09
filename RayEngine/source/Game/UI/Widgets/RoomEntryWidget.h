#pragma once
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
        TYPE_INFO(RoomEntryWidget, Container);
    public:
        RoomEntryWidget() = default;
        RoomEntryWidget(const ResRoom& InRoom) : Container(), room(InRoom) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

    private:
        ResRoom room;
    };
}
