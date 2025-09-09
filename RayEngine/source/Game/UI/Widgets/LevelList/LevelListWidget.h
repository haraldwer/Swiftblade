#pragma once
#include "Database/Events.h"
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/List.h"

namespace UI
{
    typedef DB::Event<DB::RPCLevelList>::ContextCallback<class LevelListWidget*> RPCListCallback; 
    
    class LevelListWidget : public List
    {
        TYPE_INFO(LevelListWidget, List);
        
    public:
        LevelListWidget()
        {
            onLevelList.SetContext(this);
        }
        
        LevelListWidget(const String& InList, const String& InTitle) :
            rpcList(InList),
            title(InTitle)
        {
            onLevelList.SetContext(this);
        }

        LevelListWidget(const LevelListWidget& InOther) :
            List(InOther),
            rpcList(InOther.rpcList),
            title(InOther.title),
            listID(InOther.listID),
            entries(InOther.entries),
            selectedID(InOther.selectedID),
            onLevelList(InOther.onLevelList)
        {
            onLevelList.SetContext(this);
        }
        
        LevelListWidget(LevelListWidget&&) = delete;
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        
    private:
        void ListEntries(const DB::Response<DB::RPCLevelList>& InData);
        void Clear();
        
        String rpcList; // Query for db
        String title;
        ElementID listID = -1;
        
        Map<String, ElementID> entries;
        String selectedID;
        
        RPCListCallback onLevelList;
    };
}    
