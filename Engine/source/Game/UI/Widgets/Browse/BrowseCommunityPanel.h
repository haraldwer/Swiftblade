#pragma once
#include "BrowsePanel.h"

namespace UI
{
    class BrowseCommunityPanel : public BrowsePanel
    {
        CLASS_INFO(BrowseCommunityPanel, BrowsePanel)
    public:
        void Init(Container &InOwner) override;
    };
}
