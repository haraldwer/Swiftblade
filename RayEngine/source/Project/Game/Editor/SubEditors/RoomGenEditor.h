#pragma once
#include "../RoomSubEditor.h"
#include "Game/Rooms/Generation/RoomGenPath.h"
#include "Game/Rooms/Generation/RoomGenProps.h"
#include "Game/Rooms/Generation/RoomGenVolume.h"
#include "Engine/UI/Instance.h"

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
    void Frame(bool InIsCameraControlling) override;
    void Enter() override;
    
private:
    ObjectPtr<UI::Instance> UI;
    ECS::EntityID StartEntity = ECS::InvalidID;
    ECS::EntityID EndEntity = ECS::InvalidID;
    int Seed = 0;

    enum class GenerationStage : uint8
    {
        PATH = 0,
        VOLUME,
        PROPPING,
        FINISHED
    } CurrentStage = GenerationStage::PATH;
    
    RoomGenBase* GetCurrentStage();
    void SetStage(GenerationStage InStage);

    RoomGenPath PathGen;
    RoomGenVolume VolumeGen;
    RoomGenProps PropsGen;
};
