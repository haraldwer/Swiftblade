#include "Panel.h"
#include "Manager.h"

Debug::Panel::Panel()
{
    Manager::Get().Register(this); 
}

Debug::Panel::~Panel()
{
    Manager::Get().Unregister(this); 
}

bool Debug::Panel::IsDebugPanelOpen() const
{
    return Manager::Get().IsOpen(DebugPanelName()); 
}
