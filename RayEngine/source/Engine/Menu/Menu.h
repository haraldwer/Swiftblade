#pragma once

#include "Engine/UI/Instance.h"

namespace Menu
{
    class Instance
    {
    public:
        virtual ~Instance() = default;

        virtual void Init() = 0; 
        virtual void Deinit() {} 
        virtual void Update(double InDelta) {}
        virtual void Draw();
        
        virtual bool IsBlocking() const { return true; }

    protected:
        UI::Instance UI; 
    };
}
