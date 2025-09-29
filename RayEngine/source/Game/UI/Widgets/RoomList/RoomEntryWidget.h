#pragma once
#include "Level/RoomInfo.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct RoomEntryData
    {
        RoomInfo info;
        bool add = false;
    };
    
    class RoomEntryWidget : public Container
    {
        CLASS_INFO(RoomEntryWidget, Container);
    public:
        RoomEntryWidget() : onChanged(this) {}
        RoomEntryWidget(const RoomEntryWidget& InOther) : Container(InOther), data(InOther.data), onChanged(InOther.onChanged, this) {}
        RoomEntryWidget(RoomEntryWidget&& InOther) = delete;
        RoomEntryWidget(const RoomEntryData& InData) : data(InData), onChanged(this) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        bool IsHovered() const override { return Element::IsHovered(); }
        RoomEntryData GetData() const { return data; }
        
    private:
        void UpdateInfo(const RoomEntryData& InData);
        
        RoomEntryData data;
        InstanceEvent<RoomEntryData>::ContextCallback<RoomEntryWidget*> onChanged;
    };
}
