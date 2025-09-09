#pragma once
#include "UI/Elements/Element.h"

namespace UI
{
    class Separator : public Element
    {
        TYPE_INFO(Separator, Element);
    public:
        Separator() :
            Element({
                .size = { 0, 3 },
                .alignment = {
                    Vec2F::One(),
                    Vec2F::Zero()
                },
                .padding = { 0, 3 }
            }, {
                .color = { 1, 1, 1, 1 },
                .cornerRadius = 2
            }) { }
    };
}
