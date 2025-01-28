#pragma once

template <class T>
struct BaseConfig : PropertyOwner<T>
{
    ~BaseConfig() override = default;
    virtual String Name() const = 0;
    String Path() const { return "Configs/C_" + Name() + ".json"; }

    void LoadConfig()
    {
        PropertyOwnerBase::Load(Path());
        LOG("Config loaded: " + Name());
    }

    bool SaveConfig() const
    {
        const String path = Path();
        const bool result = PropertyOwnerBase::Save(path);
        LOG((result ? "Config saved: " : "Failed to save config: ") + path);
        return result; 
    }
};
