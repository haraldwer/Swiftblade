#include "Instance.h"

#include "Elements/Container.h"

void UI::Instance::Init()
{
    Container::Init();

    // When virtual target is recreated, invalidate hierarchy
    onCreateVirtualTarget.Bind([&](const OnSetViewportSize& InData)
    {
        Invalidate(); 
    });
}

void UI::Instance::Invalidate()
{
    if (parent)
    {
        parent->Invalidate();
        return; 
    }

    RefreshRect(GetReferenceRect());
}
