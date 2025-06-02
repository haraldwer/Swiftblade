#include "Menu.h"

void Menu::Instance::Draw()
{
    if (ui)
        ui->Draw(); 
}
