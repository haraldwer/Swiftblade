#include "Manager.h"

void Debug::Manager::DrawDebugUI() const
{
    for (const auto w : Windows)
        if (w->IsDebugUIOpen())
            w->DrawDebugWindow();
}

void Debug::Manager::Register(Window* InWindow)
{
    Windows.insert(InWindow);
}

void Debug::Manager::Unregister(Window* InWindow)
{
    Windows.erase(InWindow);
}
