#pragma once
#include "Database/Events.h"
#include "Database/Data/RPCLevel.h"
#include "UI/Elements/List.h"

namespace UI
{
    typedef DB::Event<DB::RPCLevelList>::ContextCallback<class LevelListWidget*> RPCLevelListCallback; 
    
    class LevelListWidget : public List
    {
        CLASS_INFO(LevelListWidget, List);
        
    public:
        LevelListWidget() : 
            List(Transform::Fill(), { .scrollable = true }, {}, true)
        {
            onList.SetContext(this);
        }
        
        LevelListWidget(const String& InList, const String& InTitle) :
            List({}, { .scrollable = true }, {}, true),
            rpcList(InList),
            title(InTitle)
        {
            onList.SetContext(this);
        }

        LevelListWidget(const LevelListWidget& InOther) :
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
        
        LevelListWidget(LevelListWidget&&) = delete;
        void Init(Container &InOwner) override;
        void Request(const String& InList);
        
    private:
        void ListEntries(const DB::Response<DB::RPCLevelList>& InData);
        void Clear();
        
        String rpcList; // Query for db
        String title;
        ElementID listID = -1;
        
        Map<String, ElementID> entries;
        String selectedID;
        
        RPCLevelListCallback onList;
    };
}    
