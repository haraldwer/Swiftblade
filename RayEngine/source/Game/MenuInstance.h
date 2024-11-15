#pragma once
#include "Database/Manager.h"
#include "Engine/Instance/Instance.h"

class MenuInstance : public Engine::Instance 
{
public: 
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    
private:
    DB::Manager DB;
};
