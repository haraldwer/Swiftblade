#pragma once
#include "Engine/UI/Instance.h"

class MenuMain
{
public: 
    void Init();
    void Update();
    void Draw(); 
private: 
    UI::Instance UI;
};
