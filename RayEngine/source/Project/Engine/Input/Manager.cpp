#include "Manager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"

void Input::Manager::Push(const String& InContext)
{
    CHECK_ASSERT(!ContextStack.empty() && ContextStack.back() == InContext, "Context already at top");
    const auto find = Config.CachedContexts.find(InContext);
    CHECK_ASSERT(find == Config.CachedContexts.end(), "Unknown context");
    ContextStack.push_back(InContext);
}

void Input::Manager::Pop(const String& InContext)
{ 
    for (int i = static_cast<int>(ContextStack.size()) - 1; i >= 0; i--)
    {
        if (ContextStack[i] == InContext)
        {
            ContextStack.erase(ContextStack.begin() + i);
            return; 
        }
    }
    CHECK_ASSERT(true, "Failed to pop context"); 
}

const Input::Action& Input::Manager::Action(const String& InAction, const String& InContext)
{
    CHECK_ASSERT(ContextStack.empty(), "Stack empty")
    for (int i = static_cast<int>(ContextStack.size()) - 1; i >= 0; i--)
    {
        // Get context
        auto contextFind = Config.CachedContexts.find(ContextStack[i]);
        CHECK_ASSERT(contextFind == Config.CachedContexts.end(), "Unknown context");
        auto& context = Config.Contexts.Get()[contextFind->second];

        // If correct context
        if (context.Name.Get() == InContext)
        {
            // Find action
            auto actionFind = context.CachedActions.find(InAction);
            if (actionFind != context.CachedActions.end())
                return context.Actions.Get()[actionFind->second]; 
        }

        // If blocking
        if (context.Blocking)
            return Action::Invalid();
    }
    LOG("Unable to find input action " + InAction + " in context " + InContext);
    return Action::Invalid();
}

void Input::Manager::Init()
{
    Config.LoadConfig();
    Config.UpdateCache(); 
}

void Input::Manager::Update()
{
    for (auto& context : Config.Contexts.Get())
        for (auto& action : context.Actions.Get())
            UpdateAction(action);
}

void Input::Manager::UpdateAction(Input::Action& InAction)
{
    CHECK_RETURN(InAction.Key < 0);
    
    // Transitions
    switch (InAction.State)
    {
    case State::PRESSED:
        InAction.State = State::DOWN;
        break;
    case State::RELEASED:
        InAction.State = State::UP;
        break;
    }

    // State change
    const int key = InAction.Key.Get();
    bool down = false;
    float value = 0.0f; 
    switch (static_cast<KeyType>(InAction.KeyType.Get()))
    {
    case KeyType::KEYBOARD:
        down = IsKeyDown(key);
        break;
    case KeyType::GAMEPAD_BUTTON:
        down = IsGamepadButtonDown(0, key);
        break;
    case KeyType::GAMEPAD_AXIS:
        value = GetGamepadAxisMovement(0, key); 
        break;
    case KeyType::MOUSE_BUTTON:
        down = IsMouseButtonDown(key); 
        break;
    case KeyType::MOUSE_AXIS:
        switch (key)
        {
        case 0: 
            value = GetMouseDelta().x;
            break;
        case 1: 
            value = GetMouseDelta().y;
            break;
        case 2: 
            value = GetMouseWheelMoveV().x;
            break;
        case 3: 
            value = GetMouseWheelMoveV().y;
            break; 
        }
        break;
    }
    if (!down)
        down = value > InAction.Deadzone; 
    if (down != InAction.Down())
        InAction.State = down ?
            State::PRESSED : State::RELEASED;
}

void Input::Manager::DrawDebugWindow()
{
    if (ImGui::CollapsingHeader("Stack"))
    {
        if (ImGui::BeginSection("Stack2"))
        {
            for (int i = static_cast<int>(ContextStack.size()) - 1; i >= 0; i--)
            {
                const String& name = ContextStack[i]; 
                auto find = Config.CachedContexts.find(name);
                CHECK_ASSERT(find == Config.CachedContexts.end(), "Unknown context");
                Context& context = Config.Contexts.Get()[find->second];
                String text = name + (context.Blocking ? " (blocking)" : "");
                if (ImGui::TreeNodeEx((text + "##StackEntry_" + std::to_string(i)).c_str()))
                {
                    if (context.Blocking)
                        ImGui::Indent(10); 
                    ImGui::TreePop(); 
                } 
            }
        }
        ImGui::EndSection();
    }

    Config.Edit("Contexts");

    if (ImGui::Button("Save"))
        Config.SaveConfig();
}
