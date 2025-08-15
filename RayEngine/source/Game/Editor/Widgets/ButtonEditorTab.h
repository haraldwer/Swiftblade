#pragma once

#include "UI/Elements/Container.h"

namespace UI
{
    class ButtonEditorTab : public Container
    {
        TYPE_INFO(ButtonEditorTab, Container);
        
    public:
        ButtonEditorTab(const String& InOption);
        void Update(Container &InOwner) override;
        void SetSelected(bool InSelected);

    private:
        String option;
        ElementID text;
        ElementID underline;
        bool selected = false;
        float alpha = 0.0f; 
    };
}
