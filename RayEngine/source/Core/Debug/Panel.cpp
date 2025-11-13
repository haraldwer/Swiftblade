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

bool Debug::Panel::PanelBegin(bool &InOutOpen)
{
    return ImGui::Begin((PanelName() + "##Panel").c_str(), &InOutOpen);
}

void Debug::Panel::PanelEnd()
{
    ImGui::End();
}

bool Debug::Panel::IsPanelOpen() const
{
    return Manager::Get().IsOpen(PanelName()); 
}
