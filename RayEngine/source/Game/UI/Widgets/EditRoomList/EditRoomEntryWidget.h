#pragma once
#include "Editor/EditRoom.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct EditRoomEntryData
    {
        ResEditRoom resource;
        bool add = false;
    };

    class EditRoomEntryWidget : public Container
    {
        CLASS_INFO(EditRoomEntryWidget, Container);
    public:
        EditRoomEntryWidget() : onChanged(this) {}
        EditRoomEntryWidget(const EditRoomEntryWidget& InOther) : Container(InOther), data(InOther.data), onChanged(InOther.onChanged, this) {}
        EditRoomEntryWidget(EditRoomEntryWidget&& InOther) = delete;
        EditRoomEntryWidget(const EditRoomEntryData& InData) : data(InData), onChanged(this) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        bool IsHovered() const override { return Element::IsHovered(); }

    private:
        void UpdateInfo(const EditRoomEntryData& InData);
        
        EditRoomEntryData data;
        InstanceEvent<EditRoomEntryData>::ContextCallback<EditRoomEntryWidget*> onChanged;
    };
}
