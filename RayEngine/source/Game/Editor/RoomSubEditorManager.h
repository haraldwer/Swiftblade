#pragma once

#include "RoomSubEditor.h"

// Just a wrapper for all the sub-editors
class RoomSubEditorManager
{
public:
    void Init(RoomEditor* InEditor);
    void Deinit();
    void Update();
    void Frame();
    void DebugDraw();
    
    template <typename T>
    T& Get() { return editors.at(Type::GetHash<T>()).Get<T>(); }
    void SetCurrent(const Type& InType);
    Type GetCurrent() const { return currentEditor; }

    bool IgnoreSave(ECS::EntityID InID);


private: 
    Map<Utility::TypeHash, Object<RoomSubEditor>> editors;
    Type currentEditor;
    
};
