#pragma once
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct LevelEntrySelected
    {
        DB::RPCLevelList::Entry entry;
    };
    
    class LevelEntryWidget : public Container
    {
        TYPE_INFO(LevelEntryWidget, Container);
    public:
        LevelEntryWidget(const DB::RPCLevelList::Entry& InEntry) : entry(InEntry) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

        bool IsHovered() const override;
        
    private:
        DB::RPCLevelList::Entry entry;
    };
}
