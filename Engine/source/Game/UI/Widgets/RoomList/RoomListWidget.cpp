#include "RoomListWidget.h"

#include "RoomEntryWidget.h"
#include "Database/Manager.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/Separator.h"
#include "UI/Widgets/LevelRoomList/LevelRoomListWidget.h"

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
    Clear();
    rpcList = InList;
    if (!rpcList.empty())
    {
        //SetLoading(true);
        
        DB::RPCRoomList::Request request;
        request.List = rpcList;
        DB::Manager::Get().rpc.Request<DB::RPCRoomList>(request);
    }
}

void UI::RoomListWidget::Update(Container &InOwner)
{
    List::Update(InOwner);

    auto& list = Get<List>(listID);
    for (auto e : list.GetChildren())
        if (list.Get<RoomEntryWidget>(e).IsClicked())
            selectedID = e;
}

UI::RoomEntryData UI::RoomListWidget::Selected() const
{
    if (selectedID == static_cast<ElementID>(-1))
        return {};
    return Get<List>(listID).Get<RoomEntryWidget>(selectedID).GetData();
}

void UI::RoomListWidget::ListEntries(const DB::Response<DB::RPCRoomList>& InData)
{
    //SetLoading(false);
    
    if (!InData.success)
    {
        //SetError(InData.error);
        return;
    }

    CHECK_RETURN(InData.data.List.Get() != rpcList);
    
    Clear();
    auto& list = Get<List>(listID);
    for (const auto& room : InData.data.Entries.Get())
    {
        RoomEntryData data;
        data.info = room;
        RoomEntryWidget e(data);
        e.Init(*this);
        list.Add(e);
    }
}

void UI::RoomListWidget::Clear()
{
    Get<List>(listID).ClearChildren();
    selectedID = -1;
}
