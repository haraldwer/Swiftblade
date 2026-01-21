#pragma once

#include "Container.h"

namespace UI
{
    class TabContainer : public Container
    {
        CLASS_INFO(TabContainer, Container);
        
    public:
        TabContainer(const Transform& InTransform = Transform::Fill(), const Background& InBg = {}, bool InScissor = false) : Container(InTransform, InBg, InScissor) {}
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
