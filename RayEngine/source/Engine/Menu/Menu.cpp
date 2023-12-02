#include "Menu.h"

void Menu::Instance::Draw()
{
    if (UI.IsValid())
        UI.Draw(); 
}
