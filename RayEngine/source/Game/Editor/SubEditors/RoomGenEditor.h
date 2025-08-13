#pragma once
#include "../RoomSubEditor.h"
#include "Rooms/Generation/RoomGenPath.h"
#include "Rooms/Generation/RoomGenProps.h"
#include "Rooms/Generation/RoomGenVolume.h"
#include "UI/Elements/Container.h"

class RoomGenBase;

class RoomGenEditor : public RoomSubEditor
{
    friend RoomGenPath;
    friend RoomGenVolume;
    friend RoomGenProps;
    
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

    enum class GenerationStage : uint8
    {
        PATH = 0,
        VOLUME,
        PROPPING,
        FINISHED
    } currentStage = GenerationStage::PATH;
    
    RoomGenBase* GetCurrentStage();
    void SetStage(GenerationStage InStage);

    RoomGenPath pathGen = {};
    RoomGenVolume volumeGen = {};
    RoomGenProps propsGen = {};
};
