#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    struct LevelSelectedEventData
    {
        String Level;
    };

    struct RoomSelectedEventData
    {
        String Room;
    };

    typedef InstanceEvent<LevelSelectedEventData>::Callback LevelSelectCallback; 
    typedef InstanceEvent<RoomSelectedEventData>::Callback RoomSelectCallback; 
    
    class BrowsePanel : public Container
    {
        TYPE_INFO(BrowsePanel, Container);
        
    public:
        BrowsePanel() : Container(Transform::Fill()) { }

        InstanceEvent<LevelSelectedEventData> levelSelectedEvent;
        InstanceEvent<RoomSelectedEventData> roomClicked;

    protected:
        ElementID root = -1; 
    };

    
}
