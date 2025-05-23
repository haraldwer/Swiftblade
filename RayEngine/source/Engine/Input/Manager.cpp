﻿#include "Manager.h"
#include "Rendering/Manager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_themes.h"

void Input::Manager::Push(const String& InContext)
{
    const auto find = Current.CachedContexts.find(InContext);
    CHECK_ASSERT(find == Current.CachedContexts.end(), "Unknown context");
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
    CHECK_ASSERT(ContextStack.empty(), "Stack empty")
    for (int i = static_cast<int>(ContextStack.size()) - 1; i >= 0; i--)
    {
        auto& context = GetContext(ContextStack[i]);
        if (context.Name.Get() == InContext)
        {
            // Find action
            auto actionFind = context.CachedActions.find(InAction);
            if (actionFind != context.CachedActions.end())
                return context.Actions.Get()[actionFind->second];
        }
        if (context.Blocking)
            break; 
    }
    return Action::Invalid();
}

void Input::Manager::Init()
{
    Current.LoadConfig();
    Current.UpdateCache();
    Push("Default");
}

void Input::Manager::Update()
{
    PROFILE();
    for (auto& context : Current.Contexts.Get())
        for (auto& action : context.Actions.Get())
            UpdateAction(action);

    if (MouseDelta.Length() > 0.0001f)
        MouseDelta = Vec2F::Zero();
}

void Input::Manager::Frame()
{
    UpdateCursorState();
}

void Input::Manager::UpdateAction(Input::Action& InAction)
{
    CHECK_RETURN(InAction.Key < 0);
    
    // Transitions
    switch (InAction.Current)
    {
    case State::PRESSED:
        InAction.Current = State::DOWN;
        break;
    case State::RELEASED:
        InAction.Current = State::UP;
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
    if (IsWindowFocused())
    {
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
                value = MouseDelta.x;
                break;
            case 1: 
                value = MouseDelta.y;
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
    }
    if (!down)
        down = abs(value) > InAction.Deadzone;
    InAction.Value = value; 
    if (down != InAction.Down())
        InAction.Current = down ?
            State::PRESSED : State::RELEASED;
}

void Input::Manager::DrawDebugWindow()
{
    if (ImGui::Button("Load"))
        Current.LoadConfig();
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        Current.SaveConfig();
    
    static String selectedContext;  
    if (!ContextStack.empty())
    {
        ImGui::SeparatorText("Stack");
        if (ImGui::BeginListBox("##Stack", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int i = static_cast<int>(ContextStack.size()) - 1; i >= 0; i--)
            {
                const String& name = ContextStack[i]; 
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
        auto& contexts = Current.Contexts.Get();
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
            Current.UpdateCache(); 
        }
        ImGui::EndListBox();
    }
    
    if (!selectedContext.empty())
    {
        const auto find = Current.CachedContexts.find(selectedContext);
        if (find != Current.CachedContexts.end())
        {
            ImGui::SeparatorText("Context");
            Context& context = Current.Contexts.Get()[find->second];
            context.Name.Edit();
            context.Blocking.Edit();
            context.CursorVisible.Edit(); 

            static int selectedAction;
            auto& actions = context.Actions.Get();
            ImGui::Text("Actions:");
            if (ImGui::BeginListBox("##Actions", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
            {
                for (int i = 0; i < actions.size(); i++)
                {
                    const bool selected = i == selectedAction; 
                    if (ImGui::Selectable((actions[i].Name.Get() + "##" + std::to_string(i)).c_str(), selected))
                        selectedAction = i; 
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
            if (selectedAction >= 0 && selectedAction < actions.size())
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
                    const int newKey = GetKeyPressed(); 
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
                switch (action.Current)
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
                ImGui::Text("Axis: %s", Utility::ToStr(action.Value).c_str());
            }
        }
    }
}

void Input::Manager::UpdateCursorState()
{
    CHECK_RETURN(ContextStack.empty());
    
    const auto md = GetMouseDelta(); 
    MouseDelta += Vec2F( md.x, md.y );
    
    // Refresh cursor visibility
    auto& context = GetContext(ContextStack.back());
    if (context.CursorVisible == IsCursorHidden())
    {
        if (context.CursorVisible)
        {
            EnableCursor();
        }
        else
        {
            DisableCursor();
        }
    }

    if (!context.CursorVisible)
        ImGui::ClearActiveID();
}

const Input::Context& Input::Manager::GetContext(const String& InName) const
{
    const auto contextFind = Current.CachedContexts.find(InName);
    CHECK_ASSERT(contextFind == Current.CachedContexts.end(), "Unknown context");
    return Current.Contexts.Get()[contextFind->second];
}
