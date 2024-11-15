#include "MenuInstance.h"

#include "UI/Menus/MenuMain.h"

void MenuInstance::Init()
{
    Instance::Init();
    Menus.Push<MenuMain>();
}
