#pragma once
#include "UI/Elements/Container.h"

namespace Menu
{
    class Instance
    {
        BASE_TYPE_INFO(Instance);
        
    public:
        virtual ~Instance() = default;

        virtual void Init() = 0; 
        virtual void Deinit() {} 
        virtual void Update();
        virtual void Draw();
        
        virtual bool IsBlocking() const { return true; }

        void DebugDraw(int& InC);

    protected:
        UI::Container ui = {}; 
    };
}
