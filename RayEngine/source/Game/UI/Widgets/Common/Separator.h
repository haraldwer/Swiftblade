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
                .size = { 0, 2 },
                .alignment = {
                    Vec2F::One(),
                    Vec2F::Zero()
                }
            }, {
                .color = { 1, 1, 1, 1 },
                .cornerRadius = 1
            }) { }
    };
}
