#pragma once
#include "Toggle.h"
#include "UI/Elements/List.h"

namespace UI
{
    struct ToggleSelectorEntry
    {
        ButtonProperties button = {};
        ToggleProperties toggle = {};
    };
    
    class ToggleSelector : public List
    {
        CLASS_INFO(ToggleSelector, List);

    public:
        ToggleSelector(
            const Transform& InTransform = Transform::Fill(),
            const Vector<ToggleSelectorEntry>& InToggles = {},
            const ListProperties& InProperties = {
                .spacing=10,
                .direction=ListDirection::HORIZONTAL
            }) :
            List(InTransform, InProperties), toggles(InToggles) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

        String GetSelected() const;
        int GetSelectedIndex() const;
        void SetSelectedIndex(int InIndex);
        void SetSelected(String InValue);

    private:
        Vector<ToggleSelectorEntry> toggles;
        ElementID selected = -1;
    };
}
