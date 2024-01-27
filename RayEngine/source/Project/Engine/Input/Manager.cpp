#include "Manager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/imgui_themes.h"

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
    return Action::Invalid();
}

void Input::Manager::Init()
{
    Config.LoadConfig();
    Config.UpdateCache();
    Push("Default");
    Push("EditorCamera");
}

void Input::Manager::Update()
{
    for (auto& context : Config.Contexts.Get())
        for (auto& action : context.Actions.Get())
            UpdateAction(action);

    ContextStack.
}

void Input::Manager::Frame()
{
    // TODO: Handle mouse movement
    // Queue all other events
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
    if (ImGui::Button("Load"))
        Config.LoadConfig();
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        Config.SaveConfig();
    
    static String selectedContext;  
    if (!ContextStack.empty())
    {
        ImGui::SeparatorText("Stack");
        if (ImGui::BeginListBox("##Stack", ImVec2(-FLT_MIN, 3 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int i = static_cast<int>(ContextStack.size()) - 1; i >= 0; i--)
            {
                const String& name = ContextStack[i]; 
                auto find = Config.CachedContexts.find(name);
                CHECK_ASSERT(find == Config.CachedContexts.end(), "Unknown context");
                Context& context = Config.Contexts.Get()[find->second];
                String text = name + (context.Blocking ? " (blocking)" : "");
                const bool selected = context.Name.Get() == selectedContext;
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
    if (ImGui::BeginListBox("##Contexts", ImVec2(-FLT_MIN, 3 * ImGui::GetTextLineHeightWithSpacing())))
    {
        auto& contexts = Config.Contexts.Get();
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
            Config.UpdateCache(); 
        }
        ImGui::EndListBox();
    }
    
    if (!selectedContext.empty())
    {
        const auto find = Config.CachedContexts.find(selectedContext);
        if (find != Config.CachedContexts.end())
        {
            ImGui::SeparatorText("Context");
            Context& context = Config.Contexts.Get()[find->second];
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
                    ImGui::Text((String("Key: ") + std::to_string(action.Key.Get())).c_str());
                    ImGui::SameLine();
                    if (ImGui::Button("Change"))
                        changing = true;
                }
                action.Deadzone.Edit(1);

                String state;
                switch (action.State)
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
                ImGui::Text(("State: " + state).c_str());
            }
        }
    }
}

Input::Context* Input::Manager::FindContext(String InName) const
{
    
}
