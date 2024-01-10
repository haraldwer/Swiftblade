#include "DebugUIManager.h"

void DebugUIManager::DrawDebugUI() const
{
    for (const auto w : Windows)
        if (w->IsDebugUIOpen())
            w->DrawDebugUI();
}

void DebugUIManager::Register(DebugUI* InWindow)
{
    Windows.insert(InWindow);
}

void DebugUIManager::Unregister(DebugUI* InWindow)
{
    Windows.erase(InWindow);
}
