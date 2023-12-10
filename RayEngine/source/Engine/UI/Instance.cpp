#include "Instance.h"

#include "Elements/Container.h"

void UI::Instance::Invalidate()
{
    if (Parent)
    {
        Parent->Invalidate();
        return; 
    }
    
    const Rect rootRect {
        Vec2F::Zero(),
        {
            static_cast<float>(GetRenderWidth()),
            static_cast<float>(GetRenderHeight())
        }
    };
    RefreshRect(rootRect);
}
