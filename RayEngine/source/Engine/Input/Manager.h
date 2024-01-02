#pragma once
#include "Context.h"
#include "Utility/Singelton.h"

namespace Input
{
    // TODO: Shouldnt be a global singelton
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Update();
        
        void Add(const Context& InContext);
        void Remove(const String& InContext);
        const Action& Action(const String& InAction, const String& InContext = "Player");

    private:
        Vector<Context> Stack;

        // TODO: Input buffering
        // TODO: Consuming input
    };
}
