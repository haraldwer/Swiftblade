#include "LevelListWidget.h"

#include "LevelEntryWidget.h"
#include "Separator.h"
#include "Database/Manager.h"
#include "Database/Data/RPCLevelList.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"

void UI::LevelListWidget::Init(Container &InOwner)
{
    if (!title.empty())
    {
        Add(Label({}, title));
        Add(Separator());
    }

    
    listID = Add(List());
    
    List::Init(InOwner);

    //onLevelList.Bind([&](auto& InResp) { ListEntries(InResp);});
    //SetLoading(true);
    
    // Do server rpc
    DB::RPCLevelList::Request request;
    request.List = list;
    DB::Manager::Get().rpc.Request<DB::RPCLevelList>(request);
}

void UI::LevelListWidget::Update(Container &InOwner)
{
    List::Update(InOwner);

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
    
    Clear();
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

void UI::LevelListWidget::Sort(const SortMode InMode)
{
    //auto ids = levels;
    //switch (InMode)
    //{
    //    default:
    //    case SortMode::NAME:
    //        std::ranges::sort(ids, [&](const LevelEntry& InA, const LevelEntry& InB)
    //        {
    //            return InA.name < InB.name;
    //        });            
    //        break;
    //    case SortMode::CREATOR:
    //        std::ranges::sort(ids, [&](const LevelEntry& InA, const LevelEntry& InB)
    //        {
    //            return InA.creator < InB.creator;
    //        });
    //        break;
    //    case SortMode::PLAYS:
    //        std::ranges::sort(ids, [&](const LevelEntry& InA, const LevelEntry& InB)
    //        {
    //            return InA.plays < InB.plays;
    //        });
    //        break;
    //    case SortMode::FAVOURITE:
    //        std::ranges::sort(ids, [&](const LevelEntry& InA, const LevelEntry& InB)
    //        {
    //            if (InA.fav == InB.fav)
    //                return InA.name < InB.name;
    //            return InA.fav; 
    //        });
    //        break;
    //}
    //SetEntries(ids);
}
