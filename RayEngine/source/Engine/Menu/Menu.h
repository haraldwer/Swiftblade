#pragma once
#include "UI/Elements/Container.h"

namespace Menu
{
    class Instance
    {
    public:
        virtual ~Instance() = default;

        virtual void Init() = 0; 
        virtual void Deinit() {} 
        virtual void Update() {}
        virtual void Draw();
        
        virtual bool IsBlocking() const { return true; }

    protected:
        UI::Container ui = {}; 
    };
}
