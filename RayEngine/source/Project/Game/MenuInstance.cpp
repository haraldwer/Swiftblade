#include "MenuInstance.h"

#include "UI/Menus/MenuMain.h"

void MenuInstance::Init()
{
    Instance::Init();
    Menus.Push<MenuMain>();
}

void MenuInstance::Update(const double InDelta)
{
    Instance::Update(InDelta); 
}

void MenuInstance::Draw()
{
    Menus.Draw(); 
}
