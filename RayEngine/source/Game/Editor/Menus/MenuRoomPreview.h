#pragma once
#include "Instance/Instance.h"

class MenuRoomPreview : public Menu::Instance
{
    TYPE_INFO(MenuRoomPreview, Menu::Instance);
public:
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return false; }

    void BeginLoading();
    void SetSubmitState(bool InSuccess, const String& InError);
    
    struct OnClickedEvent
    {
        String option;
    };
};
