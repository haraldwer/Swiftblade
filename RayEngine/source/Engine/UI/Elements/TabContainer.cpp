#include "TabContainer.h"

void UI::TabContainer::Update(Container &InOwner)
{
    Container::Update(InOwner);

    // TODO: Animate between selected entries
    previous.clear();

    for (auto& c : children)
        Get<Element>(c).SetVisible(c == selected);
}

void UI::TabContainer::SetIndex(int InIndex)
{
    CHECK_ASSERT(InIndex < 0 || InIndex >= static_cast<int>(children.size()), "Invalid index");
    Set(children.at(InIndex));
}

void UI::TabContainer::Set(const String& InSelected)
{
    Set(GetID(InSelected));
}

void UI::TabContainer::Set(ElementID InID)
{
    if (InID != selected)
    {
        if (selected != -1)
        {
            auto& prev = previous.emplace_back();
            prev.id = selected;
            prev.timestamp = static_cast<float>(Utility::Time::Get().Total());
        }
        selected = InID;
    }
}
