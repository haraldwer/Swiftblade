#pragma once

#include "Tone.h"

class ToneLibrary : public PropertyOwner<ToneLibrary>
{
public:

    bool Edit(const String &InName, uint32 InOffset) override;
    bool Contains(const String& InName);
    Tone& Get(const String& InName);

private:
    PROPERTY(Vector<Tone>, Tones);
    PROPERTY_D(int, EditedTone, -1);
    
    void UpdateCache();
    Map<String, int> toneCache;
};