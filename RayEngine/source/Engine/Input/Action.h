#pragma once

#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"

namespace Input
{
    enum class State
    {
        UP,
        PRESSED,
        DOWN,
        RELEASED
    };
    
    struct Action : PropertyOwner<Action>
    {
        friend class Manager; 
        
        bool Pressed() const { return Valid && State == State::PRESSED; }
        bool Down()  const { return Valid && State == State::PRESSED || State == State::DOWN; } 
        bool Released() const { return Valid && State == State::RELEASED; }
        float Axis() const { return static_cast<float>(Valid) * Value * static_cast<float>(Down()); }

        PROPERTY(String, Name); 
        PROPERTY_D(int, Key, 0);
        
        static Action& Invalid()
        {
            static Action a;
            a.Valid = false; 
            return a; 
        }

    private:
        bool Valid = true; 
        State State = State::UP;
        float Value = 0.0f; 
    };
}