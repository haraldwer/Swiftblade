#pragma once
#include "../RoomSubEditor.h"
#include "Generation/RoomGenVolume.h"
#include "UI/Elements/Container.h"

class RoomGenBase;

class RoomGenEditor : public RoomSubEditor
{
    CLASS_INFO(RoomGenEditor, RoomSubEditor);
    
public: 
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Enter() override;
    bool IsEnabled(const String& InRoomType) override;

    void StartGen();

private:
    RoomGenVolume volumeGenerator;
    bool generating = false;
};
