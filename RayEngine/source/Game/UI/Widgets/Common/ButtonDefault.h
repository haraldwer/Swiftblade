#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    class ButtonDefault : public Container
    {
        CLASS_INFO(ButtonDefault, Container);
    public:
        ButtonDefault(const Transform& InTrans, const String& InOption) : Container(InTrans), option(InOption), text(InOption) {}
        ButtonDefault(const Transform& InTrans, const String& InOption, const String& InText) : Container(InTrans), option(InOption), text(InText) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        bool IsHovered() const override { return Element::IsHovered(); }

    private:
        String option;
        String text;
        ElementID root = -1;
    };
}
