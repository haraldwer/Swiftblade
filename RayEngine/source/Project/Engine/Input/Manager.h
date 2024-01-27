#pragma once

#include "Config.h"
#include "Utility/Singelton.h"
#include "Engine/Editor/Debugging/Window.h"

namespace Input
{
    class Manager : public Utility::Singelton<Manager>, public Debug::Window
    {
    public:
        void Init();
        void Update();
        void Frame();
        void UpdateAction(Action& InAction);
        
        void Push(const String& InContext);
        void Pop(const String& InContext);
        
        const Action& Action(const String& InAction, const String& InContext);

        String DebugWindowName() const override { return "Input"; }
        void DrawDebugWindow() override;

    private:
        
        Vector<String> ContextStack;
        Config Config; 

        // TODO: Input buffering
        // TODO: Consuming input
    };
}
