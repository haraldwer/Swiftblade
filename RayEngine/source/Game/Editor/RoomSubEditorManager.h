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
    void SetCurrent(const String& InName);
    Type GetCurrent() const { return currentEditor; }
    String GetCurrentName() const;

    bool IgnoreSave(ECS::EntityID InID);


private: 
    Map<Utility::TypeHash, Object<RoomSubEditor>> editors;
    Type currentEditor;
    RoomType roomType;
    
    InstanceEvent<MenuRoomEditor::OnClickedEvent>::Callback onMenuClicked;
};
