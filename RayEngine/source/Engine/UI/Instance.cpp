#include "Instance.h"

#include "Elements/Container.h"

void UI::Instance::Init()
{
    CHECK_RETURN(!Root);
    Root->Init(); 
}

void UI::Instance::Update()
{
    CHECK_RETURN(!Root);
    Root->Update();
}

void UI::Instance::Draw()
{
    CHECK_RETURN(!Root);    
    Root->Draw(); 
}
