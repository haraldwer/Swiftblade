#pragma once

#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"

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
    
    struct Action : PropertyOwner<Action>
    {
        friend Manager; 
        friend Context; 
        
        bool Pressed() const { return Key >= 0 && State == State::PRESSED; }
        bool Down()  const { return Key >= 0 && State == State::PRESSED || State == State::DOWN; } 
        bool Released() const { return Key >= 0 && State == State::RELEASED; }
        float Axis() const { return static_cast<float>(Key >= 0) * Value * static_cast<float>(Down()); }

        operator bool() const { return Key >= 0; }
        
        static Action& Invalid()
        {
            static Action a;
            return a;  
        }

        static const Action& Get(const String& Action, const String& Context = "");

    private:
        
        PROPERTY_D(String, Name, "Unnamed"); 
        PROPERTY_D(int, Key, -1);
        PROPERTY_D(uint8, KeyType, 0)
        PROPERTY_D(float, Deadzone, 0.1f)
        
        State State = State::UP;
        float Value = 0.0f; 
    };
}
