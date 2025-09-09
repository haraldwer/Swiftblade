#include "InfoPanelLevel.h"

void UI::InfoPanelLevel::Init(Container &InOwner)
{
    Container::Init(InOwner);
}

void UI::InfoPanelLevel::SetLevel(const DB::RPCLevelList::Entry &InLevel)
{
    SetVisible(true);
}

void UI::InfoPanelLevel::Hide()
{
    SetVisible(false);
}
