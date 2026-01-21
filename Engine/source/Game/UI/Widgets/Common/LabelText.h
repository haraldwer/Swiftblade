#pragma once
#include "UI/Elements/Label.h"

namespace UI
{
    class LabelText : public Label
    {
        CLASS_INFO(LabelText, Label);
    public:
        LabelText(const Transform& InTrans = {}, const String& InText = {}) :
            Label(InTrans, {
                InText,
                static_cast<float>(LabelSize::TEXT)
            }) { }
    };
}
