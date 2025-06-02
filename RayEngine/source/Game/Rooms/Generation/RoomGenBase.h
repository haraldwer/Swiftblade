#pragma once

class RoomGenEditor;

class RoomGenBase
{
public:
    virtual ~RoomGenBase() = default;
    virtual void Clear() = 0;
    virtual void Init() = 0;
    virtual bool Step() = 0;
    
    void SetOwner(RoomGenEditor* InEditor) { owner = InEditor; }
    
protected:
    RoomGenEditor* owner = nullptr;
};
