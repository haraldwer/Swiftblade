#include "RoomListWidget.h"

#include "RoomEntryWidget.h"
#include "Database/Manager.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/Separator.h"

void UI::RoomListWidget::Init(Container &InOwner)
{
    if (!title.empty())
    {
        Add(LabelHeader({}, title));
        Add(Separator());
    }
    
    listID = Add(List());
    
    List::Init(InOwner);

    onList.Bind([&](auto& InResp, auto InContext)
    {
        InContext->ListEntries(InResp);
    });
    
    // Do server rpc
    Request(rpcList);
}

void UI::RoomListWidget::Request(const String &InList)
{
    //Clear();
    rpcList = InList;
    if (!rpcList.empty())
    {
        //SetLoading(true);
        
        DB::RPCRoomList::Request request;
        request.List = rpcList;
        DB::Manager::Get().rpc.Request<DB::RPCRoomList>(request);
    }
}

void UI::RoomListWidget::ListEntries(const DB::Response<DB::RPCRoomList> &InData)
{
    //SetLoading(false);
    
    if (!InData.success)
    {
        //SetError(InData.error);
        return;
    }

    CHECK_RETURN(InData.data.List.Get() != rpcList);
    
    //Clear();
    for (const DB::RPCRoomList::Entry& room : InData.data.Entries.Get())
    {
        RoomEntryData data;
        data.entry = room;
        RoomEntryWidget e(data);
        e.Init(*this);
        entries[room.ID] = Get<List>(listID).Add(e);
    }
}

void UI::RoomListWidget::Clear()
{
    CHECK_RETURN(entries.empty())
    Get<List>(listID).ClearChildren();
    entries.clear();
    selectedID = {};
}