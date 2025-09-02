#pragma once
#include "Database/Events.h"
#include "Database/Data/Auth.h"
#include "Database/Data/RPCUser.h"
#include "Instance/Instance.h"

class MenuLogin : Menu::Instance
{
public:
    void Init() override;
    void Update() override;

private:
    void LoginStep();
    void BeginSelectUsername();
    void SetLoginFailed(const String& InError);
    
    DB::Event<DB::AuthResponse>::Callback onAuth;
    DB::Event<DB::RPCLogin>::Callback onLogin;
    bool loading = false;
};
