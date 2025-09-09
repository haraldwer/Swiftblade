#pragma once
#include "UI/Elements/Label.h"

namespace UI
{
    class LabelTitle : public Label
    {
        TYPE_INFO(LabelTitle, Label);
    public:
        LabelTitle(const Transform& InTrans = {}, const String& InText = {}) :
            Label(InTrans, {
                InText,
                static_cast<float>(LabelSize::TITLE),
                0,
                ResFont("UI/F_LinLibertine_aSZI.ttf")
            }) { }
    };
}
