#pragma once

namespace Input
{
    class Manager;
    struct Context;
    
    enum class State
    {
        UP,
        PRESSED,
        DOWN,
        RELEASED
    };

    enum class KeyType : uint8
    {
        KEYBOARD = 0,
        GAMEPAD_BUTTON,
        GAMEPAD_AXIS,
        MOUSE_BUTTON,
        MOUSE_AXIS
    };
    
    class Action : public PropertyOwner<Action>
    {
        friend Manager; 
        friend Context; 
     
    public:
        
        bool Pressed() const { return Key >= 0 && current == State::PRESSED; }
        bool Down()  const { return Key >= 0 && (current == State::PRESSED || current == State::DOWN); } 
        bool Released() const { return Key >= 0 && current == State::RELEASED; }
        float Axis() const { return static_cast<float>(Key >= 0) * value * static_cast<float>(Down()); }

        operator bool() const { return Key >= 0; }
        
        static Action& Invalid()
        {
            static Action a;
            return a;  
        }

        static const Action& Get(const String& InAction, const String& InContext = "");

    private:
        
        PROPERTY_D(String, Name, "Unnamed"); 
        PROPERTY_D(int, Key, -1);
        PROPERTY_D(uint8, KeyType, 0)
        PROPERTY_D(float, Deadzone, 0.1f)
        
        State current = State::UP;
        float value = 0.0f; 
    };
}
