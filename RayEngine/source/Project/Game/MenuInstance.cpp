#include "MenuInstance.h"

#include "UI/Menus/MenuMain.h"

void MenuInstance::Init()
{
    Menus.Push<MenuMain>();
}

void MenuInstance::Update(const double InDelta)
{
    Menus.Update(InDelta); 
}

void MenuInstance::DrawUI()
{
    Menus.Draw(); 
}
