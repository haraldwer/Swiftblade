#pragma once
#include "../RoomSubEditor.h"
#include "UI/Elements/Container.h"

class RoomGenBase;

class RoomGenEditor : public RoomSubEditor
{
    TYPE_INFO(RoomGenEditor, RoomSubEditor);
    
public: 
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Frame() override;
    void Enter() override;
    
private:
    UI::Container ui = {};
    ECS::EntityID startEntity = ECS::INVALID_ID;
    ECS::EntityID endEntity = ECS::INVALID_ID;
    int seed = 0;
};
