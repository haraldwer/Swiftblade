#pragma once
#include "DebugUI.h"
#include "Utility/Singelton.h"

class DebugUIManager : public Utility::Singelton<DebugUIManager, true>
{
public:
    void DrawDebugUI() const;
    
    void Register(DebugUI* InWindow);
    void Unregister(DebugUI* InWindow);

private:
    Set<DebugUI*> Windows; 
};
