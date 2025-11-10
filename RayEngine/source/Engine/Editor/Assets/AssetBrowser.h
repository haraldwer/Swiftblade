#pragma once
#include <filesystem>

#include "AssetDetails.h"
#include "Debug/Panel.h"

class AssetBrowser : public Debug::Panel
{
public:
    void Init();
    void Deinit();
    
    String DebugPanelName() const override { return "Assets"; }
    void DrawDebugPanel() override;
    
    void SelectAsset(const String& InPath);
    
private:
    
    AssetDetails details;

#ifdef IMGUI_ENABLE
    
    struct Node
    {
        std::filesystem::path path;
        String name;
        String ext;
        std::chrono::system_clock::time_point date;
        int size = 0;
        bool dir = false;
        Vector<Node> children;
        
        bool operator==(const Node& InNode) const = default;
    } root;
    
    bool running = false;
    std::thread thread;
    std::mutex rootLock;
    bool editLocked = false;

    enum class Sort
    {
        NAME = 0,
        DATE,
        SIZE
    } sort = Sort::NAME;
    bool sortRev = false;

    void TryStartThread();
    void TryStopThread();
    
    void ForceUpdateCache();
    void UpdateCache(const std::filesystem::path& InPath, Node& InNode);
    std::function<bool(const Node &, const Node &)> GetSortFunc() const;
    void SelectNode(const Node& InNode);

    void NodeDragDrop(const Node& InNode);
    void NodeContext(const Node& InNode);
    void KeyboardShortcuts();
    
    void BeginCut();
    void BeginCopy();
    void Paste();
    void Duplicate();
    void Delete();
    
    void DrawNode(Node& InNode);
    void Search(const String& InStr);
    void DrawAddMenu();

    Set<std::filesystem::path> expanded;
    Set<std::filesystem::path> selected;

    String search;
    Vector<String> searchSplit;
    Set<std::filesystem::path> searchResult;

    String addName = "";
    String addPath = "";
    bool openAdd = false;

#endif
};
