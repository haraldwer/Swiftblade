#pragma once
#include "LevelEntryWidget.h"
#include "UI/Elements/List.h"

namespace UI
{
    class LevelListWidget : public List
    {
        TYPE_INFO(LevelListWidget, List);
        
    public:
        LevelListWidget(const String& InListing, const String& InTitle) : listing(InListing), title(InTitle) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        String GetSelected() const { return selectedID; }
        
    private:
        void Clear();
        void SetEntries(const Vector<LevelEntry> &InEntries);
        List& GetList();
        
        enum class SortMode : uint8
        {
            NAME = 0,
            CREATOR,
            PLAYS,
            FAVOURITE
        };
        
        void Sort(SortMode InMode);
        
        String listing; // Query for db
        String title;
        ElementID rootID = -1;
        ElementID listID = -1;
        
        Map<String, ElementID> entries;
        String selectedID;
    };
}    
