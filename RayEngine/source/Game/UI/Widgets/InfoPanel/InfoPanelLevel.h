#pragma once
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/Container.h"

namespace UI
{
    struct LevelEntrySelected;
    
    class InfoPanelLevel : public Container
    {
        CLASS_INFO(InfoPanelLevel, Container);
    public:
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetLevel(const LevelEntrySelected& InLevel);
    };
}
