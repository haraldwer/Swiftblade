#include "DebugUI.h"

#include "DebugUIManager.h"

DebugUI::DebugUI()
{
    DebugUIManager::Get().Register(this); 
}

DebugUI::~DebugUI()
{
    DebugUIManager::Get().Unregister(this); 
}
