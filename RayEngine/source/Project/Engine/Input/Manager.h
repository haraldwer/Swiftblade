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

        const Action& GetAction(const String& InAction, const String& InContext = "") const;

        String DebugWindowName() const override { return "Input"; }
        void DrawDebugWindow() override;

    private:
        const Action& GetActionInternal(const String& InAction, const String& InContext) const;

        void UpdateCursorState();
        const Context& GetContext(const String& InName) const;
        
        Vector<String> ContextStack;
        Config Config; 
        Vec2F MouseDelta;

        // TODO: Input buffering
        // TODO: Consuming input
    };
}
