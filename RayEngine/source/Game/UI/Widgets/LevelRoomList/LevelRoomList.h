#pragma once

#include "Level/Room.h"
#include "UI/Elements/List.h"

namespace UI
{
    class LevelRoomList : public List
    {
        CLASS_INFO(LevelRoomList, List);
        
    public:
        LevelRoomList(const Transform& InTransform = Transform::Fill()) : List(InTransform, { .scrollable = true }, {}, true) {}
        
        void Init(Container &InOwner) override;
        void SetEntries(const Vector<RoomEntry>& InEntries);

    private:
        Vector<RoomEntry> entries;
    };
}
