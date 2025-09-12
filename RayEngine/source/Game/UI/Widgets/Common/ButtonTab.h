#pragma once

#include "UI/Elements/Container.h"

namespace UI
{
    class ButtonTab : public Container
    {
        CLASS_INFO(ButtonTab, Container);
        
    public:
        ButtonTab(const String& InOption) : Container(), option(InOption) { }
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
