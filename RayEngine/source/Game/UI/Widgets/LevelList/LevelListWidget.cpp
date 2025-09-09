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
    
    //SetLoading(true);
    
    // Do server rpc
    if (!rpcList.empty())
    {
        DB::RPCLevelList::Request request;
        request.List = rpcList;
        DB::Manager::Get().rpc.Request<DB::RPCLevelList>(request);
    }

    onLevelList.Bind([&](auto& InResp, auto InContext)
    {
        InContext->ListEntries(InResp);
    });
}

void UI::LevelListWidget::Update(Container &InOwner)
{
    List::Update(InOwner);

    return;
    auto& l = Get<List>(listID);
    for (auto& eID : entries)
    {
        if (l.Get<LevelEntryWidget>(eID.second).IsClicked())
        {
            selectedID = eID.first;
        }
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
        LevelEntryWidget e(level);
        e.Init(*this);
        entries[level.ID] = Get<List>(listID).Add(e);
    }
}

void UI::LevelListWidget::Clear()
{
    Get<List>(listID).ClearChildren();
    entries.clear();
    selectedID = {};
}
