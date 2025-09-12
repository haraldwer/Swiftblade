#pragma once
#include "UI/Elements/Label.h"

namespace UI
{
    class LabelHeader : public Label
    {
        CLASS_INFO(LabelHeader, Label);
    public:
        LabelHeader(const Transform& InTrans = {}, const String& InText = {}) :
            Label(InTrans, {
                InText,
                static_cast<float>(LabelSize::HEADER),
                ResFont("UI/F_LinLibertine_aSZI.ttf")
            }) { }
    };
}
