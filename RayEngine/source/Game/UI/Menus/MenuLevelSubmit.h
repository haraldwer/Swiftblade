#pragma once
#include "Instance/Instance.h"

class MenuLevelSubmit : public Menu::Instance
{
    CLASS_INFO(MenuLevelSubmit, Menu::Instance);
public:
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return false; }
    bool CanBePopped() const override { return false; }

    void BeginLoading();
    void SetSubmitState(bool InSuccess, const String& InError);
    
    struct OnClickedEvent
    {
        String option;
    };
};
