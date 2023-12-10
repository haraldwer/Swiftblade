#include "Menu.h"

void Menu::Instance::Draw()
{
    if (UI)
        UI->Draw(); 
}
