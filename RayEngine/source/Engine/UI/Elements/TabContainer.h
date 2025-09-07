#pragma once

#include "Container.h"

namespace UI
{
    class TabContainer : public Container
    {
        TYPE_INFO(TabContainer, Container);
        
    public:
        TabContainer() = default;
        TabContainer(const Transform& InTransform) : Container(InTransform) {}
        void Update(Container& InOwner) override;

        void SetIndex(int InIndex);
        void Set(const String& InSelected);
        
    private:
        void Set(ElementID InID);
        ElementID selected = -1;
        
        struct Entry
        {
            ElementID id;
            float timestamp;
        };
        Vector<Entry> previous;
    };
}
