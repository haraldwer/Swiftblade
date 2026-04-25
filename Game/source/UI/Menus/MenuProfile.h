#pragma once
#include "Database/Events.h"
#include "../../Database/Data/RPCUserInfo.h"
#include "Instance/Instance.h"

class MenuProfile : Menu::Instance
{
    CLASS_INFO(MenuProfile, Menu::Instance);
public:
    MenuProfile() = default;
    MenuProfile(const String &InUser) : Instance(), user(InUser) {}
    
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return true; };
    
private:
    void RequestUserInfo();
    String user;

    DB::Event<DB::RPCUserInfo>::Callback onRecieveInfo;
    DB::Event<DB::RPCUserLB>::Callback onRecieveLB;
    DB::Event<DB::RPCUserFavs>::Callback onRecieveFavs;
    DB::Event<DB::RPCUserSubmission>::Callback onRecieveSubmissions;
};
