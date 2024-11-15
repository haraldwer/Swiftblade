#include "Instance.h"

#include "Elements/Container.h"

void UI::Instance::Init()
{
    Container::Init();

    // When virtual target is recreated, invalidate hierarchy
    OnCreateVirtualTarget.Bind([&](const OnSetViewportSize& InData)
    {
        Invalidate(); 
    });
}

void UI::Instance::Invalidate()
{
    if (Parent)
    {
        Parent->Invalidate();
        return; 
    }

    RefreshRect(GetReferenceRect());
}
