#pragma once
#include "UI/Elements/Element.h"

namespace UI
{
    class Spacing : public Element
    {
        TYPE_INFO(Spacing, Element);
        
    public:
        Spacing(float InSize = 15) :
            Element({
                .size = { InSize }
            }) { }
    };
}
