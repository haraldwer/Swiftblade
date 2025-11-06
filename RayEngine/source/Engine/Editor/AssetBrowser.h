#pragma once
#include <filesystem>

#include "Debug/Panel.h"

class AssetBrowser : public Debug::Panel
{
public:
    void Init();
    void Deinit();
    String DebugPanelName() const override { return "Assets"; }
    void DrawDebugPanel() override;

private:

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

    enum class Sort
    {
        NAME = 0,
        DATE,
        SIZE
    } sort = Sort::NAME;
    bool sortRev = false;
    
    void UpdateCache(const std::filesystem::path& InPath, Node& InNode);
    std::function<bool(const Node &, const Node &)> GetSortFunc() const;
    void SelectNode(const Node& InNode);
    void DrawNode(Node& InNode);
    void Search(const String& InStr);

    Set<std::filesystem::path> expanded;

    String search;
    Vector<String> searchSplit;
    Set<std::filesystem::path> searchResult;

#endif
};
