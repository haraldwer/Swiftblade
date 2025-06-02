#pragma once

#include "Config.h"
#include "Core/Debug/Panel.h"

namespace Input
{
    class Manager : public Utility::Singelton<Manager>, public Debug::Panel
    {
    public:
        void Init();
        void Update();
        void Frame();
        void UpdateAction(Action& InAction) const;
        
        void Push(const String& InContext);
        void Pop(const String& InContext);

        const Action& GetAction(const String& InAction, const String& InContext = "") const;

        String DebugPanelName() const override { return "Input"; }
        void DrawDebugPanel() override;

    private:
        const Action& GetActionInternal(const String& InAction, const String& InContext) const;

        void UpdateCursorState();
        const Context& GetContext(const String& InName) const;
        
        Vector<String> contextStack = {};
        Config current = {}; 
        Vec2F mouseDelta = {};

        // TODO: Input buffering
        // TODO: Consuming input
    };
}
