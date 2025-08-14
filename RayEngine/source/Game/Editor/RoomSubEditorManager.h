#pragma once

#include "RoomSubEditor.h"
#include "Menus/MenuRoomEditor.h"

// Just a wrapper for all the sub-editors
class RoomSubEditorManager
{
public:
    void Init(RoomEditor* InEditor);
    void Deinit();
    void Update();
    void Frame();
    void DebugDraw();
    
    template <typename EditorT>
    EditorT& Get()
    {
        Object<RoomSubEditor>& obj = editors.at(Type::GetHash<EditorT>());
        return obj.Get<EditorT>();
    }
    
    void SetCurrent(const Type& InType);
    Type GetCurrent() const { return currentEditor; }

    bool IgnoreSave(ECS::EntityID InID);


private: 
    Map<Utility::TypeHash, Object<RoomSubEditor>> editors;
    Map<Utility::TypeHash, String> typeToName;
    Map<String, Utility::TypeHash> nameToType;
    Type currentEditor;
    
    InstanceEvent<MenuRoomEditor::OnClickedEvent>::Callback OnMenuClicked;
};
