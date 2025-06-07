#include "Instance.h"

#include "Elements/Container.h"

void UI::Instance::Init()
{
    Init(*this);
}

void UI::Instance::Invalidate()
{
    Invalidate(*this);
}

void UI::Instance::Update()
{
    Container::Update(*this);
}

void UI::Instance::Draw()
{
    Draw(*this);
}

void UI::Instance::Init(Instance &InInstance)
{
    Container::Init(InInstance);

    if (parent == -1) // This is root instance
    {
        // When virtual target is recreated, invalidate hierarchy
        onSetViewportSize.Bind([&](const OnSetViewportSize& InData)
        {
            Invalidate(); 
        });
    }
}
void UI::Instance::Invalidate(Instance &InInstance)
{
    Container::Invalidate(InInstance);
    
    if (parent == -1) // This is root instance
        RefreshRect(*this, GetReferenceRect());
}

void UI::Instance::Draw(Instance &InInstance)
{
    Container::Draw(InInstance);
}
