#pragma once

struct Note
{
    
};

struct Rythm
{
    // A pattern that notes can be placed on
    // Different densities?
    // Note timing
    // Note length

    // 1. At current position, randomize density
    // 2. Place beat, move to beat
    // Repeat


    void Func()
    {
        
    }
};

class Track : public PropertyOwner<Track>
{
public:

    void Add(const Note& InNote);
    
private:

    // Notes are just rythm beats but with a pitch
    String toneName;
};
