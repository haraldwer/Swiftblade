#pragma once
#include "Instance/Instance.h"

class MenuProfile : Menu::Instance
{
public:
    MenuProfile() = default;
    MenuProfile(const String &InUser) : Instance(), user(InUser) {}
    
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return true; };
    
private:
    void RequestUserInfo();
    void ApplyUserInfo();
    String user;
    
};
