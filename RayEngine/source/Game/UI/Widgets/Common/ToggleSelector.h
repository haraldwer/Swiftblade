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
        ToggleSelector(const Transform& InTransform = Transform::Fill(), const Vector<ToggleSelectorEntry>& InToggles = {}, const ListProperties& InProperties = { 10, ListDirection::HORIZONTAL }) : List(InTransform, InProperties), toggles(InToggles) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;

        String GetSelected() const;

    private:
        Vector<ToggleSelectorEntry> toggles;
        ElementID selected = -1;
    };
}
