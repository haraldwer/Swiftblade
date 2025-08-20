#pragma once

#include "UI/Elements/Container.h"

namespace UI
{
    class TabButton : public Container
    {
        TYPE_INFO(TabButton, Container);
        
    public:
        TabButton(const String& InOption) : Container(), option(InOption) { }
        void Init(Container& InOwner) override;
        void Update(Container &InOwner) override;
        void SetSelected(bool InSelected);

    private:
        String option;
        ElementID text = -1;
        ElementID underline = -1;
        bool selected = false;
        float alpha = 0.0f; 
    };
}
