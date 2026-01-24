#pragma once

#include "Property/PropertyOwner.h"

template <class T>
struct BaseConfig : PropertyOwner<T>
{
    ~BaseConfig() override = default;
    virtual String Name() const = 0;
    String Path() const { return "Configs/C_" + Name() + ".json"; }

    bool Edit(const String& InName = "", uint32 InOffset = 0) override
    {
        const bool result = PropertyOwner<T>::Edit(InName, InOffset);
        if (Utility::Button("Save", InOffset))
            SaveConfig();
        Utility::SameLine();
        if (Utility::Button("Load", InOffset))
            LoadConfig();
        return result;
    }
    
    void LoadConfig()
    {
        if (PropertyOwnerBase::Load(Path()))
            LOG("Config loaded: " + Name())
        else
            LOG("Failed to load config")
    }

    bool SaveConfig() const
    {
        const String path = Path();
        const bool result = PropertyOwnerBase::Save(path);
        LOG((result ? "Config saved: " : "Failed to save config: ") + path);
        return result; 
    }
};
