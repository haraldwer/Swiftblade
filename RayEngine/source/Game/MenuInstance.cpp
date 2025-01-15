#include "MenuInstance.h"

#include "UI/Menus/MenuMain.h"

void MenuInstance::Init()
{
    Instance::Init();
    Menus.Push<MenuMain>();
    DB.Init();

    DB::AuthData auth;
    auth.User = "TestUser";
    auth.Create = true;
    DB.Auth.Authenticate(auth);
}

void MenuInstance::Deinit()
{
    Instance::Deinit();
    DB.Deinit();
}

void MenuInstance::Logic(double InDelta)
{
    Instance::Logic(InDelta);
    DB.Update();
}
