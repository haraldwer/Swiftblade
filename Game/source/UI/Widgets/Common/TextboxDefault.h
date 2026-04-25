#pragma once
#include "UI/Elements/Textbox.h"

namespace UI
{
    class TextboxDefault : public Textbox
    {
        CLASS_INFO(TextboxDefault, Textbox);
    public:
        TextboxDefault(const Transform& InTrans = {}, const TextboxProperties& InProperties = {
            {
                    "",
                    static_cast<float>(LabelSize::TEXT)
                },
                "",
                "",
                false,
                false,
                0,
            }) : Textbox(InTrans, InProperties, {{ 1, 1, 1, 0.5 }}) {}
    };
}
