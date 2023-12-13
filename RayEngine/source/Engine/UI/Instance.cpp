#include "Instance.h"

#include "Elements/Container.h"

void UI::Instance::Invalidate()
{
    if (Parent)
    {
        Parent->Invalidate();
        return; 
    }

    RefreshRect(GetReferenceRect());
}
