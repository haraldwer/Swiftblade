#pragma once
#include "BrowsePanel.h"

namespace UI
{
    class BrowseCreatePanel : public BrowsePanel
    {
        TYPE_INFO(BrowseCreatePanel, BrowsePanel);

    public:
        void Init(Container &InOwner) override;


        void Update(Container &InOwner) override;

    private:
        void SelectLevels();
        void SelectRooms();
        void SelectSubmissions();
    };
}
