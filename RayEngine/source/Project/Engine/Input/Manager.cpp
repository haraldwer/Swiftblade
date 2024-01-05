#include "Manager.h"

void Input::Manager::Add(const Context& InContext)
{
    Stack.push_back(InContext);
    Stack.back().UpdateCache(); 
}

void Input::Manager::Remove(const String& InContext)
{
    CHECK_ASSERT(Stack.empty(), "Stack empty");
    for (int i = static_cast<int>(Stack.size()) - 1; i >= 0; i--)
    {
        if (Stack[i].Name.Get() == InContext)
        {
            // Only erase one
            Stack.erase(Stack.begin() + i);
            return;
        } 
    }
}

const Input::Action& Input::Manager::Action(const String& InAction, const String& InContext)
{
    CHECK_ASSERT(Stack.empty(), "Stack empty")
    for (int i = static_cast<int>(Stack.size()) - 1; i >= 0; i--)
    {
        auto& context = Stack[i];
        if (context.Name.Get() == InContext)
        {
            auto find = context.CachedActions.find(InAction);
            if (find != context.CachedActions.end())
                return context.Actions.Get()[find->second]; 
        }
        if (context.Blocking)
            return Action::Invalid();  
    }
    LOG("Unable to find input action " + InAction + " in context " + InContext);
    return Action::Invalid();
}

void Input::Manager::Update()
{
    for (auto& context : Stack)
    {
        for (auto& action : context.Actions.Get())
        {
            // Transitions
            switch (action.State)
            {
            case State::PRESSED:
                action.State = State::DOWN;
                break;
            case State::RELEASED:
                action.State = State::UP;
                break;
            }

            // State change
            const int key = action.Key.Get();
            const bool down = IsKeyDown(key);
            if (down != action.Down())
                action.State = down ?
                    State::PRESSED : State::RELEASED;

            // Axis?
            // Mouse input? 
        }
    }
}
