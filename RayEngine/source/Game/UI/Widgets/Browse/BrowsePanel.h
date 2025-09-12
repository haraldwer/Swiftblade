#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    class BrowsePanel : public Container
    {
        CLASS_INFO(BrowsePanel, Container);
        
    public:
        BrowsePanel() : Container(Transform::Fill()) { }

    protected:
        ElementID root = -1; 
    };
}
