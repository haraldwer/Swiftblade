#include "Manager.h"
#include "Rendering/Manager.h"

#ifdef IMGUI_ENABLE
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#endif

void Input::Manager::Push(const String& InContext)
{
    LOG("Pushing context: " + InContext);
    const auto find = current.cachedContexts.find(InContext);
    CHECK_ASSERT(find == current.cachedContexts.end(), "Unknown context");
    contextStack.push_back(InContext);
}

void Input::Manager::Pop(const String& InContext)
{
    LOG("Popping context: " + InContext);
    for (int i = static_cast<int>(contextStack.size()) - 1; i >= 0; i--)
    {
        if (contextStack[i] == InContext)
        {
            contextStack.erase(contextStack.begin() + i);
            return; 
        }
    }
    CHECK_ASSERT(true, "Failed to pop context"); 
}


const Input::Action& Input::Manager::GetAction(const String& InAction, const String& InContext) const
{
    if (!InContext.empty())
        return GetActionInternal(InAction, InContext);
    if (auto& playerAction = GetActionInternal(InAction, "Player"))
        return playerAction;
    if (auto& defaultAction = GetActionInternal(InAction, "Default"))
        return defaultAction;
    return Action::Invalid();
}

const Input::Action& Input::Manager::GetActionInternal(const String& InAction, const String& InContext) const
{
    CHECK_ASSERT(contextStack.empty(), "Stack empty")
    CHECK_RETURN(!Rendering::Manager::Get().IsViewportClickable(), Action::Invalid());
    for (int i = static_cast<int>(contextStack.size()) - 1; i >= 0; i--)
    {
        auto& context = GetContext(contextStack[i]);
        if (context.Name.Get() == InContext)
        {
            // Find action
            auto actionFind = context.cachedActions.find(InAction);
            if (actionFind != context.cachedActions.end())
                return context.Actions.Get()[actionFind->second];
        }
        if (context.Blocking)
            break; 
    }
    return Action::Invalid();
}

void Input::Manager::Init()
{
    current.LoadConfig();
    current.UpdateCache();
    Push("Default");
}

void Input::Manager::Update()
{
    PROFILE();
    for (auto& context : current.Contexts.Get())
        for (auto& action : context.Actions.Get())
            UpdateAction(action);

    if (mouseDelta.Length() > 0.0001f)
        mouseDelta = Vec2F::Zero();
}

void Input::Manager::Frame()
{
    UpdateCursorState();
}

void Input::Manager::UpdateAction(Action& InAction) const
{
    CHECK_RETURN(InAction.Key < 0);
    
    // Transitions
    switch (InAction.current)
    {
    case State::PRESSED:
        InAction.current = State::DOWN;
        break;
    case State::RELEASED:
        InAction.current = State::UP;
        break;
    case State::UP:
    case State::DOWN:
    default:
        break;
    }

    // State change
    const int key = InAction.Key.Get();
    bool down = false;
    float value = 0.0f;
    if (Rendering::Manager::Get().IsViewportClickable())
    {
        /*
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
                value = mouseDelta.x;
                break;
            case 1: 
                value = mouseDelta.y;
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

#ifdef IMGUI_ENABLE

        if (static_cast<KeyType>(InAction.KeyType.Get()) == KeyType::MOUSE_BUTTON)
        {
            switch (key)
            {
                case MOUSE_BUTTON_LEFT: down |= ImGui::IsMouseDown(ImGuiMouseButton_Left); break;
                case MOUSE_BUTTON_RIGHT: down |= ImGui::IsMouseDown(ImGuiMouseButton_Right); break;
                case MOUSE_BUTTON_MIDDLE: down |= ImGui::IsMouseDown(ImGuiMouseButton_Middle); break;
            }
        }
        
#endif
        */
        
    }
    if (!down)
        down = abs(value) > InAction.Deadzone;
    InAction.value = value; 
    if (down != InAction.Down())
        InAction.current = down ?
            State::PRESSED : State::RELEASED;
}

#ifdef IMGUI_ENABLE

void Input::Manager::DrawPanel()
{
    if (current.ConsumeCursor.Edit())
        current.SaveConfig();
    
    static String selectedContext;  
    if (!contextStack.empty())
    {
        ImGui::SeparatorText("Stack");
        if (ImGui::BeginListBox("##Stack", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int i = static_cast<int>(contextStack.size()) - 1; i >= 0; i--)
            {
                const String& name = contextStack[i]; 
                auto& context = GetContext(name);
                const String text = name + (context.Blocking ? " (blocking)" : "");
                const bool selected = name == selectedContext;
                if (ImGui::Selectable((text + "##StackEntry_" + std::to_string(i)).c_str(), selected))
                {
                    selectedContext = context.Name; 
                    if (context.Blocking)
                        ImGui::Indent(10); 
                } 
            }
            ImGui::EndListBox();
        }
    }

    ImGui::SeparatorText("Contexts");
    if (ImGui::BeginListBox("##Contexts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
    {
        auto& contexts = current.Contexts.Get();
        for (Context& context : contexts)
        {
            const bool selected = context.Name.Get() == selectedContext; 
            if (ImGui::Selectable((context.Name.Get() + "##ContextEntry").c_str(), selected))
            {
                selectedContext = context.Name; 
                if (context.Blocking)
                    ImGui::Indent(10); 
            }
        }
        if (ImGui::Button("Add"))
        {
            Context c;
            c.Name = "Unnamed";
            contexts.push_back(c);
            current.UpdateCache(); 
        }
        ImGui::EndListBox();
    }
    
    if (!selectedContext.empty())
    {
        const auto find = current.cachedContexts.find(selectedContext);
        if (find != current.cachedContexts.end())
        {
            ImGui::SeparatorText("Context");
            Context& context = current.Contexts.Get()[find->second];
            context.Name.Edit();
            context.Blocking.Edit();
            context.CursorVisible.Edit(); 

            static int selectedAction;
            auto& actions = context.Actions.Get();
            ImGui::Text("Actions:");
            if (ImGui::BeginListBox("##Actions", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
            {
                for (size_t i = 0; i < actions.size(); i++)
                {
                    const bool selected = static_cast<int>(i) == selectedAction; 
                    if (ImGui::Selectable((actions[i].Name.Get() + "##" + std::to_string(i)).c_str(), selected))
                        selectedAction = static_cast<int>(i); 
                }
                if (ImGui::Button("Add"))
                {
                    actions.emplace_back();
                    context.UpdateCache();
                }
                if (!actions.empty())
                {
                    ImGui::SameLine();
                   if (ImGui::Button("Remove"))
                   {
                       actions.erase(actions.begin() + selectedAction);
                       context.UpdateCache(); 
                   }
                } 
                ImGui::EndListBox();
            }

            // Edit selected action
            if (selectedAction >= 0 && selectedAction < static_cast<int>(actions.size()))
            {
                ImGui::SeparatorText("Action");
                auto& action = actions[selectedAction];
                action.Name.Edit(1);
                
                static bool changing = false;
                const char* keyTypes[]
                {
                    "KEYBOARD",
                    "GAMEPAD_BUTTON",
                    "GAMEPAD_AXIS",
                    "MOUSE_BUTTON",
                    "MOUSE_AXIS"
                };

                int keyType = action.KeyType.Get();
                ImGui::ListBox("Type", &keyType, keyTypes, 5, 5);
                action.KeyType = static_cast<uint8>(keyType); 
                
                if (changing)
                {
                    ImGui::Text("Changing key..."); 
                    const int newKey = 0;// TODO: GetKeyPressed(); 
                    if (newKey)
                    {
                        changing = false;
                        action.Key = newKey; 
                    }
                }
                else
                {
                    ImGui::Text("Key: %s", Utility::ToStr(action.Key.Get()).c_str());
                    ImGui::SameLine();
                    if (ImGui::Button("Change"))
                        changing = true;
                }
                action.Deadzone.Edit(1);

                String state;
                switch (action.current)
                {
                case State::UP:
                    state = "UP";
                    break;
                case State::PRESSED:
                    state = "PRESSED";
                    break;
                case State::DOWN:
                    state = "DOWN";
                    break;
                case State::RELEASED:
                    state = "RELEASED";
                    break;
                }
                ImGui::Text("State: %s", state.c_str());
                ImGui::Text("Axis: %s", Utility::ToStr(action.value).c_str());
            }
        }
    }

    if (ImGui::Button("Load"))
        current.LoadConfig();
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        current.SaveConfig();
}

#else
void Input::Manager::DrawDebugPanel() {  }
#endif

void Input::Manager::UpdateCursorState()
{
    CHECK_RETURN(contextStack.empty());
    /*
    const auto md = GetMouseDelta(); 
    mouseDelta += Vec2F( md.x, md.y );
    
    // Refresh cursor visibility
    auto& context = GetContext(contextStack.back());
    if (context.CursorVisible == IsCursorHidden())
    {
        if (context.CursorVisible)
        {
            EnableCursor();
        }
        else if (current.ConsumeCursor)
        {
            DisableCursor();
        }
    }

#ifdef IMGUI_ENABLE
    if (!context.CursorVisible)
        ImGui::ClearActiveID();
#endif
    */
}

const Input::Context& Input::Manager::GetContext(const String& InName) const
{
    const auto contextFind = current.cachedContexts.find(InName);
    CHECK_ASSERT(contextFind == current.cachedContexts.end(), "Unknown context");
    return current.Contexts.Get()[contextFind->second];
}
