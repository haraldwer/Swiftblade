#include "LevelListWidget.h"

#include "LevelEntryWidget.h"
#include "Database/Manager.h"
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/Label.h"
#include "UI/Widgets/Common/LabelHeader.h"
#include "UI/Widgets/Common/Separator.h"

void UI::LevelListWidget::Init(Container &InOwner)
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

void UI::LevelListWidget::Request(const String &InList)
{
    //Clear();
    rpcList = InList;
    if (!rpcList.empty())
    {
        //SetLoading(true);
        
        DB::RPCLevelList::Request request;
        request.List = rpcList;
        DB::Manager::Get().rpc.Request<DB::RPCLevelList>(request);
    }
}

void UI::LevelListWidget::ListEntries(const DB::Response<DB::RPCLevelList>& InData)
{
    //SetLoading(false);
    
    if (!InData.success)
    {
        //SetError(InData.error);
        return;
    }

    CHECK_RETURN(InData.data.List.Get() != rpcList);
    
    //Clear();
    for (const DB::RPCLevelList::Entry& level : InData.data.Entries.Get())
    {
        LevelEntryData data;
        data.entry = level;
        LevelEntryWidget e(data);
        e.Init(*this);
        entries[level.ID] = Get<List>(listID).Add(e);
    }
}

void UI::LevelListWidget::Clear()
{
    CHECK_RETURN(entries.empty())
    Get<List>(listID).ClearChildren();
    entries.clear();
    selectedID = {};
}
