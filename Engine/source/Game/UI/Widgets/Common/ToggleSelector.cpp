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

    auto childrenIDs = GetChildren();
    if (childrenIDs.empty())
        return;

    if (selected == static_cast<ElementID>(-1))
        selected = childrenIDs.front();
    for (auto e : childrenIDs)
        if (Get<Toggle>(e).IsClicked())
            selected = e;
    for (auto e : childrenIDs)
        Get<Toggle>(e).SetToggled(e == selected);
}

String UI::ToggleSelector::GetSelected() const
{
    if (selected == static_cast<ElementID>(-1))
        return "";
    return Get<Toggle>(selected).GetOption();
}

int UI::ToggleSelector::GetSelectedIndex() const
{
    auto childrenIDs = GetChildren();
    for (int i = 0; i < static_cast<int>(childrenIDs.size()); i++)
        if (childrenIDs[i] == selected)
            return i;
    return 0;
}

void UI::ToggleSelector::SetSelectedIndex(const int InIndex)
{
    auto childrenIDs = GetChildren();
    for (int i = 0; i < static_cast<int>(childrenIDs.size()); i++)
    {
        auto childID = childrenIDs[i];
        Get<Toggle>(childID).SetToggled(i == InIndex);
        if (i == InIndex)
            selected = childID;
    }
}


void UI::ToggleSelector::SetSelected(const String &InValue)
{
    for (auto e : children)
    {
        auto& t = Get<Toggle>(e);
        if (t.GetOption() == InValue)
        {
            t.SetToggled(true);
            selected = e;
        }
        else
        {
            t.SetToggled(false);
        }
    }
}
