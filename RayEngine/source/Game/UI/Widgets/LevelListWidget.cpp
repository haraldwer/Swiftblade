#include "LevelListWidget.h"

#include "LevelEntryWidget.h"
#include "Separator.h"
#include "UI/Builder.h"
#include "UI/Elements/Label.h"

void UI::LevelListWidget::Init(Container &InOwner)
{
    List::Init(InOwner);

    Builder b = Builder();
    if (!title.empty())
    {
        b.Add(Label({}, title), "Title");
        b.Add(Separator());
    }
    b.Add(List(), "List");
    rootID = Add(b.Build());
    listID = Get<List>(rootID).GetID("List");

    
}

void UI::LevelListWidget::Update(Container &InOwner)
{
    List::Update(InOwner);

    selectedID = {};
    auto& list = GetList();
    for (auto& eID : entries)
        if (list.Get<LevelEntryWidget>(eID.second).IsClicked())
            selectedID = eID.first;
}

void UI::LevelListWidget::Clear()
{
    GetList().ClearChildren();
    entries.clear();
    selectedID = {};
}

void UI::LevelListWidget::SetEntries(const Vector<LevelEntry> &InEntries)
{
    Clear();
    for (auto& level : InEntries)
    {
        LevelEntryWidget e(level);
        e.Init(*this);
        entries[level.id] = GetList().Add(e);
    }
}

UI::List& UI::LevelListWidget::GetList()
{
    return Get<List>(rootID).Get<List>(listID); 
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
