#pragma once
#include "Database/Events.h"
#include "Database/Data/RPCRoom.h"
#include "Instance/Instance.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"

class MenuLevelPlay : public Menu::Instance
{
    CLASS_INFO(MenuLevelPlay, Menu::Instance);
public:
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return true; }
    bool CanBePopped() const override { return false; }
    void SetLevel(const UI::LevelEntryData& InData, int InSeed = 0);

private:

    void BeginLoading();
    
    void ReceiveLevel(const DB::Response<DB::RPCLevelInfo>& InResponse);
    void SetLevel(const Level& InData);
    
    void RequestRoom(const String& InRoom);
    void ReceiveRoom(const DB::Response<DB::RPCRoomInfo>& InResponse);
    
    void TryFinishRoomLoading();

    bool loading = false; 
    
    UI::LevelEntryData levelEntry;
    LevelConfig config;
    DB::Event<DB::RPCLevelInfo>::Callback onLevelInfo;
    
    Set<String> pendingRoomRequests;
    Map<String, ResScene> sceneResources;
    Vector<String> rooms;
    Vector<String> arenas;
    DB::Event<DB::RPCRoomInfo>::Callback onRoomInfo;
};
