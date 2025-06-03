#include "MenuInstance.h"

#include "UI/Menus/MenuMain.h"

void MenuInstance::Init()
{
    Instance::Init();
    menus.Push<MenuMain>();
    db.Init();

    DB::AuthData auth;
    auth.User = "TestUser";
    auth.create = true;
    db.auth.Authenticate(auth);
}

void MenuInstance::Deinit()
{
    Instance::Deinit();
    db.Deinit();
}

void MenuInstance::Logic(const double InDelta)
{
    Instance::Logic(InDelta);
    db.Update();
}
