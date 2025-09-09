#pragma once
#include "Database/Data/RPCLevelList.h"
#include "UI/Elements/Container.h"

namespace UI
{
    class InfoPanelLevel : public Container
    {
        TYPE_INFO(InfoPanelLevel, Container);
    public:
        void Init(Container &InOwner) override;
        void SetLevel(const DB::RPCLevelList::Entry& InLevel);
        void Hide(); 
    };
}
