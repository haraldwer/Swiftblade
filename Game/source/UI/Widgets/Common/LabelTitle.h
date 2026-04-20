#pragma once
#include "UI/Elements/Label.h"

namespace UI
{
    class LabelTitle : public Label
    {
        CLASS_INFO(LabelTitle, Label);
    public:
        LabelTitle(const Transform& InTrans = {}, const String& InText = {}, const Background& InBg = {}) :
            Label(InTrans, {
                InText,
                static_cast<float>(LabelSize::TITLE),
                ResFont("UI/F_LinLibertine_aSZI.ttf")
            }, InBg) { }
    };
}
