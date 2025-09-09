#include "Menu.h"

void Menu::Instance::Update()
{
    ui.Update();
}

void Menu::Instance::Draw()
{
    ui.Draw(); 
}

void Menu::Instance::DebugDraw(int& InC)
{
    ui.DebugDraw(InC);
}
