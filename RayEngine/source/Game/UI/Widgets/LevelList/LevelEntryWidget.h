#pragma once
#include "Database/Data/RPCLevelList.h"
#include "Level/Level.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct LevelEntrySelected
    {
        DB::RPCLevelList::Entry entry;
        ResLevel level;
        bool add;
    };
    
    class LevelEntryWidget : public Container
    {
        CLASS_INFO(LevelEntryWidget, Container);
    public:
        LevelEntryWidget() = default;
        LevelEntryWidget(const ResLevel& InLevel) : levelResource(InLevel) {}
        LevelEntryWidget(const DB::RPCLevelList::Entry& InEntry) : listEntry(InEntry) {}
        LevelEntryWidget(bool InAdd) : add(InAdd) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

        bool IsHovered() const override;
        
    private:
        void RefreshInfo();
        
        ResLevel levelResource;
        DB::RPCLevelList::Entry listEntry;
        bool add = false;
    };
}
