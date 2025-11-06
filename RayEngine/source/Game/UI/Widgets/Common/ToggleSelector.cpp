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

int UI::ToggleSelector::GetSelectedIndex() const
{
    auto children = GetChildren();
    for (int i = 0; i < children.size(); i++)
        if (children[i] == selected)
            return i;
    return 0;
}

void UI::ToggleSelector::SetSelectedIndex(int InIndex)
{
    auto children = GetChildren();
    for (int i = 0; i < children.size(); i++)
    {
        auto id = children[i];
        Get<Toggle>(id).SetToggled(i == InIndex);
        if (i == InIndex)
            selected = id;
    }
}


void UI::ToggleSelector::SetSelected(String InValue)
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
