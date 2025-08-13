#pragma once

#include "RoomSubEditor.h"
#include "Utility/History/History.h"

// Just a wrapper for all the sub-editors
class RoomSubEditorManager
{
public:
    void Init();
    void Deinit();
    void Update();
    void Frame();
    void DebugDraw();
    
    template <class T>
    T& Get() { return editors.at(Type::Get<T>()).Get(); }
    void SetCurrent(const Type& InType);

    bool IgnoreSave(ECS::EntityID InID);
    
private: 
    
    Map<Type, Object<RoomSubEditor>> editors;
    Type currentEditor;
    
};
