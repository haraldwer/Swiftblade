#pragma once
#include "Instance/Instance.h"

class MenuBrowse : public Menu::Instance
{
public:
    void Init() override;
    void SetPanelType(const Type& InType) { panelType = InType; }

private:
    Type panelType;
};
