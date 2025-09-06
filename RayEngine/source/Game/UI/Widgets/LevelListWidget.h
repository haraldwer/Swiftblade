#pragma once
#include "Database/Events.h"
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/List.h"

namespace UI
{
    class LevelListWidget : public List
    {
        TYPE_INFO(LevelListWidget, List);
        
    public:
        LevelListWidget(const String& InList, const String& InTitle) : list(InList), title(InTitle) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        
    private:
        void ListEntries(const DB::Response<DB::RPCLevelList>& InData);
        void Clear();
        
        enum class SortMode : uint8
        {
            NAME = 0,
            CREATOR,
            PLAYS,
            FAVOURITE
        };
        
        void Sort(SortMode InMode);
        
        String list; // Query for db
        String title;
        ElementID listID = -1;
        
        Map<String, ElementID> entries;
        String selectedID;
        DB::Event<DB::RPCLevelList>::Callback onLevelList;
    };
}    
