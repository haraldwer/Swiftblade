#pragma once
#include "../RoomSubEditor.h"

class RoomGenEditor : public RoomSubEditor
{
public: 
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Frame(bool InIsCameraControlling) override;
    
    void Enter() override;
    
    void GeneratePath();
    void GenerateVolume();
    void GenerateProps();
    
private:
    
    ObjectPtr<UI::Instance> UI;
    
    ECS::EntityID StartEntity = ECS::InvalidID;
    ECS::EntityID EndEntity = ECS::InvalidID;

    enum class GenerationStage : uint8
    {
        PATH = 0,
        VOLUME,
        PROPPING,
        FINISHED
    } CurrentState = GenerationStage::PATH;
    
    Vector<Coord> Path;
    Set<uint32> PathSet;

    struct QueuedEntry
    {
        uint32 Coord = 0;
        uint32 Ref = 0;
    };
    
    Vector<QueuedEntry> QueuedCoords;
    Map<uint32, uint32> NextQueue;
    Map<uint32, Set<uint32>> CheckedCoords;
    Map<uint32, uint8> Result;
    
    int VolumeDepth = 0; 
    
};
