#pragma once
#include "Database/Data/RPCLevel.h"
#include "Level/Level.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct LevelEntryData
    {
        DB::RPCLevelList::Entry entry;
        ResLevel resource;
        bool add = false;
    };
    
    class LevelEntryWidget : public Container
    {
        CLASS_INFO(LevelEntryWidget, Container);
    public:
        LevelEntryWidget() : onChanged(this) {}
        LevelEntryWidget(const LevelEntryWidget& InOther) : Container(InOther), data(InOther.data), onChanged(InOther.onChanged, this) {}
        LevelEntryWidget(LevelEntryWidget&& InOther) = delete;
        LevelEntryWidget(const LevelEntryData& InData) : data(InData), onChanged(this) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        bool IsHovered() const override { return Element::IsHovered(); }
        
    private:
        void UpdateInfo(const LevelEntryData& InData);
        
        LevelEntryData data;
        Engine::InstanceEvent<LevelEntryData>::ContextCallback<LevelEntryWidget*> onChanged;
    };
}
