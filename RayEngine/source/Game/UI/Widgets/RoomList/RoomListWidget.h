#pragma once
#include "Database/Events.h"
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/List.h"

namespace UI
{
    typedef DB::Event<DB::RPCRoomList>::ContextCallback<class RoomListWidget*> RPCRoomListCallback; 
    
    class RoomListWidget : public List
    {
    public:
        RoomListWidget() :
            List({}, { .scrollable = true }, {}, true)
        {
            onList.SetContext(this);
        }

        RoomListWidget(const String& InList, const String& InTitle) :
            List({}, { .scrollable = true }, {}, true),
            rpcList(InList),
            title(InTitle)
        {
            onList.SetContext(this);
        }
        
        RoomListWidget(const RoomListWidget& InOther) :
            List(InOther),
            rpcList(InOther.rpcList),
            title(InOther.title),
            listID(InOther.listID),
            entries(InOther.entries),
            selectedID(InOther.selectedID),
            onList(InOther.onList)
        {
            onList.SetContext(this);
        }
        
        RoomListWidget(RoomListWidget&&) = delete;
        void Init(Container &InOwner) override;
        void Request(const String& InList);        

    private:
        void ListEntries(const DB::Response<DB::RPCRoomList>& InData);
        void Clear();
        
        String rpcList; // Query for db
        String title;
        ElementID listID = -1;
        
        Map<String, ElementID> entries;
        String selectedID;
        
        RPCRoomListCallback onList;
    };
}
