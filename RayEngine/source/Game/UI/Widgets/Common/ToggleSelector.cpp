#include "ToggleSelector.h"

void UI::ToggleSelector::Init(Container &InOwner)
{
    for (auto t : toggles)
        Add(Toggle({}, t.button, t.toggle));
    List::Init(InOwner);
}

void UI::ToggleSelector::Update(Container &InOwner)
{
    List::Update(InOwner);

    auto children = GetChildren();
    if (children.empty())
        return;

    if (selected == -1)
        selected = children.front();
    for (auto e : children)
        if (Get<Toggle>(e).IsClicked())
            selected = e;
    for (auto e : children)
        Get<Toggle>(e).SetToggled(e == selected);
}

String UI::ToggleSelector::GetSelected() const
{
    if (selected == -1)
        return "";
    return Get<Toggle>(selected).GetOption();
}
