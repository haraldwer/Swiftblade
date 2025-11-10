#include "AssetBrowser.h"

#include <filesystem>
#include <ranges>

#include "Utility/String/StringConversionTime.h"
#include "Collections/SortedInsert.h"
#include "ImGui/imgui.h"

#ifdef IMGUI_ENABLE

void AssetBrowser::Init()
{
    expanded.insert(std::filesystem::current_path());
    TryStartThread();
}

void AssetBrowser::Deinit()
{
    TryStopThread();
}

void AssetBrowser::TryStartThread()
{
    if (!running)
    {
        running = true;
        thread = std::thread([&]
        {
            while (running)
            {
                Node newRoot;
                UpdateCache(std::filesystem::current_path(), newRoot);
                rootLock.lock();
                if (root != newRoot)
                {
                    root = newRoot;
                    Search(search);
                }
                rootLock.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }
}

void AssetBrowser::TryStopThread()
{
    if (running)
    {
        running = false;
        thread.join();
    }
}

void AssetBrowser::ForceUpdateCache()
{
    if (!editLocked)
        rootLock.lock();
    Node newRoot;
    UpdateCache(std::filesystem::current_path(), newRoot);
    root = newRoot;
    if (!editLocked)
        rootLock.unlock();
}

void AssetBrowser::UpdateCache(const std::filesystem::path& InPath, Node& InNode)
{
    InNode.path = InPath;
    InNode.name = Utility::StringRemove(InPath.filename().string(), InPath.extension().string());
    InNode.ext = InPath.extension().string();
    InNode.dir = is_directory(InPath);
    if (!InNode.dir)
        InNode.size = static_cast<int>(file_size(InPath));
    InNode.date = std::chrono::clock_cast<std::chrono::system_clock>(last_write_time(InPath));

    if (!InNode.dir)
        return;

    std::function<bool(const Node&, const Node&)> sortFunc = GetSortFunc();
    for (auto& path : std::filesystem::directory_iterator(InPath))
    {
        Node childNode;
        UpdateCache(path, childNode);
        Utility::SortedInsert(InNode.children, childNode, sortFunc);
    }

    if (sortRev)
    {
        auto rev = std::ranges::views::reverse(InNode.children);
        InNode.children = { rev.begin(), rev.end() };
    }
}

void AssetBrowser::DrawDebugPanel()
{
    ImGui::SetNextItemWidth(-1);
    String newSearch = search;
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Search: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##AssetSearch", &newSearch, ImGuiInputTextFlags_AutoSelectAll);
    if (newSearch != search)
        Search(newSearch);
    
    if (ImGui::BeginTable("Assets", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed);

        auto header = [&](String InName, Sort InMode)
        {
            ImGui::TableHeader(InName.c_str());
            if (ImGui::IsItemClicked())
            {
                if (sort == InMode)
                    sortRev = !sortRev;
                else
                {
                    sort = InMode;
                    sortRev = false;
                }
                ForceUpdateCache();
            }
        };

        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableSetColumnIndex(0);
        header("Name", Sort::NAME);
        ImGui::TableSetColumnIndex(1);
        header("Size", Sort::SIZE);
        ImGui::TableSetColumnIndex(2);
        header("Date", Sort::DATE);

        rootLock.lock();
        editLocked = true;
        DrawNode(root);
        rootLock.unlock();
        editLocked = false;
        ImGui::EndTable();
    }

    DrawAddMenu();
}

void AssetBrowser::SelectAsset(const String &InPath)
{
    std::function<bool(const Node&)> select = [&](const Node& InNode)
    {
        if (InNode.path == InPath)
        {
            SelectNode(InNode);
            return true;
        }

        if (InNode.dir)
            for (const Node &c: InNode.children)
                if (select(c))
                    return true;

        return false;
    };
    select(root);
}

std::function<bool(const AssetBrowser::Node&, const AssetBrowser::Node&)> AssetBrowser::GetSortFunc() const
{
    std::function<bool(const Node&, const Node&)> sortFunc;
    switch (sort)
    {
    case Sort::NAME:
        sortFunc = [](const Node& InA, const Node& InB) { return InA.name.compare(InB.name) < 0; };
        break;
    case Sort::DATE:
        sortFunc = [](const Node& InA, const Node& InB) { return InA.date < InB.date; };
        break;
    case Sort::SIZE:
        sortFunc = [](const Node& InA, const Node& InB) { return InA.size < InB.size; };
        break;
    }
    return sortFunc;
}

void AssetBrowser::SelectNode(const Node& InNode)
{
    bool singleSelect = !ImGui::IsKeyDown(ImGuiKey_LeftShift);
    bool alreadySelected = selected.contains(InNode.path);
    bool openDirectly = selected.empty();
    
    if (singleSelect)
        selected.clear();
    selected.insert(InNode.path);

    if (InNode.dir)
    {
        if (singleSelect)
        {
            if (!expanded.contains(InNode.path) || !alreadySelected)
                expanded.insert(InNode.path);
            else expanded.erase(InNode.path);
        }
    }
    else if (alreadySelected || openDirectly)
    {
        if (singleSelect)
            details.Clear();
        details.Select({ Utility::RelativePath(InNode.path.string()) });
    }
}

void AssetBrowser::NodeDragDrop(const Node &InNode)
{
    //// Drag drop
    //if (ImGui::BeginDragDropSource())
    //{
    //    // Set dragdrop payload!
    //    ImGui::SetDragDropPayload("asset", &String(InNode.path), sizeof(String));
    //    ImGui::EndDragDropSource();
    //}
    //if (ImGui::BeginDragDropTarget())
    //{
    //    const ImGuiPayload* p = ImGui::AcceptDragDropPayload("asset");
    //    ImGui::EndDragDropTarget();
    //}
}

void AssetBrowser::NodeContext(const Node &InNode)
{
    if (ImGui::BeginPopupContextItem(InNode.path.c_str()))
    {
        if (!selected.contains(InNode.path))
            SelectNode(InNode);
        
        if (selected.size() <= 1) ImGui::SeparatorText(InNode.name.c_str());
        else ImGui::SeparatorText((Utility::ToStr(selected.size()) + " assets").c_str());

        if (ImGui::Selectable("Open##Popup"))
        {
            details.Clear();
            for (auto& s : selected)
                if (!is_directory(s))
                    details.Select({ Utility::RelativePath(s.string()) });
        }

        if (ImGui::Selectable("Add##Popup"))
        {
            if (InNode.dir)
            {
                addPath = InNode.path;
                addName = "";
            }
            else
            {
                addPath = InNode.path.parent_path();
                addName = InNode.name + InNode.ext;
            }
            
            openAdd = true;
        }
        
        ImGui::Separator();
        
        if (ImGui::Selectable("Cut##Popup")) BeginCut();
        if (ImGui::Selectable("Copy##Popup")) BeginCopy();
        if (ImGui::Selectable("Paste##Popup")) Paste();
        if (ImGui::Selectable("Duplicate##Popup")) Duplicate();
        if (ImGui::Selectable("Delete##Popup")) Delete();

        ImGui::EndPopup();
    }
}

void AssetBrowser::KeyboardShortcuts()
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_C)) BeginCopy();
        if (ImGui::IsKeyPressed(ImGuiKey_X)) BeginCut();
        if (ImGui::IsKeyPressed(ImGuiKey_V)) Paste();
        if (ImGui::IsKeyPressed(ImGuiKey_D)) Duplicate();
        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) Delete();
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_Enter))
        for (auto& s : selected)
            if (!is_directory(s))
                details.Select({ Utility::RelativePath(s) });
}

void AssetBrowser::BeginCut()
{
    LOG("Begin cut");
    String cutCmd = "CUT:";
    for (auto& s : selected)
        cutCmd += s.string() + ";";
    ImGui::SetClipboardText(cutCmd.c_str());
}

void AssetBrowser::BeginCopy()
{
    LOG("Begin copy");
    String copyCmd = "CPY:";
    for (auto& s : selected)
        copyCmd += s.string() + ";";
    ImGui::SetClipboardText(copyCmd.c_str());
}

void AssetBrowser::Paste()
{
    String clip = ImGui::GetClipboardText();
    if (clip.starts_with("CUT:") || clip.starts_with("CPY:"))
    {
        Set<std::filesystem::path> targetDirs;
        if (selected.empty())
            return;
        for (auto& s : selected)
            if (is_directory(s)) targetDirs.insert(s);
            else targetDirs.insert(s.parent_path());
        
        String content = clip.substr(4);
        Utility::StringSplit(content, ";", [&](const String& sourcePath)
        {
            String name = Utility::Filename(sourcePath);
            for (auto& t : targetDirs)
            {
                String targetPath = t.string() + "/" + name;
                Utility::CopyFile(sourcePath, targetPath, true);
            }

            if (clip.starts_with("CUT:"))
                Utility::DeleteFile(sourcePath);
        });

        ForceUpdateCache();
        ImGui::SetClipboardText("");
    }
}

void AssetBrowser::Duplicate()
{
    for (auto& s : selected)
        Utility::CopyFile(s, s, true);
    ForceUpdateCache();
}

void AssetBrowser::Delete()
{
    for (auto& s : selected)
        Utility::DeleteFile(s);
    ForceUpdateCache();
}

void AssetBrowser::DrawNode(Node& InNode)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    bool found = searchResult.empty() || searchResult.contains(InNode.path);
    if (!found)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128 ,255));

    bool select = selected.contains(InNode.path);
    float height = ImGui::CalcTextSize("T").y + 2;
    String itemName = InNode.name + "##" + InNode.path.string();
    if (ImGui::Selectable(itemName.c_str(), select, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, height)))
        SelectNode(InNode);
    if (ImGui::IsItemHovered())
        KeyboardShortcuts();
    NodeContext(InNode);
    NodeDragDrop(InNode);
    
    ImGui::SameLine();
    if (found)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128 ,255));
    ImGui::Text(InNode.ext.c_str());

    ImGui::TableNextColumn();
    if (InNode.size > 0)
        ImGui::Text(Utility::ToStr(InNode.size).c_str());
    ImGui::TableNextColumn();

    ImGui::Text(Utility::ToStr(InNode.date).c_str());
    ImGui::PopStyleColor();

    if (InNode.dir && expanded.contains(InNode.path))
    {
        ImGui::Indent();
        for (auto& child : InNode.children)
            DrawNode(child);
        ImGui::Unindent();
    }
}

void AssetBrowser::Search(const String& InStr)
{
    search = InStr;
    searchSplit = Utility::StringSplit(InStr, " ");
    searchResult.clear();
    if (!searchSplit.empty())
        expanded.clear();

    std::function<bool(const Node&)> func;
    func = [&](const Node& InNode)
    {
        bool result = false;
        for (auto s : searchSplit)
        {
            if ((InNode.name + InNode.ext).contains(s))
            {
                searchResult.insert(InNode.path);
                result = true;
            }
        }
        if (InNode.dir)
            for (auto& child : InNode.children)
                result |= func(child);
        if (result)
            expanded.insert(InNode.path);
        return result;
    };
    func(root);
}

void AssetBrowser::DrawAddMenu()
{
    if (openAdd)
    {
        ImGui::OpenPopup("Create asset");
        openAdd = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create asset", nullptr))
    {
        ImGui::Text("Filename");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##Filename", &addName);

        String path = addPath + "/" + addName;
        ImGui::Text("Path: %s", Utility::RelativePath(path).c_str());
        
        if (addName.empty())
        {
            ImGui::Text("Error: No name specified");
        }
        else if (Utility::FileExists(path))
        {
            ImGui::Text("Error: File already exists");
        }
        else if (ImGui::Button("Create##Asset", ImVec2(-1, 0)))
        {
            Utility::WriteFile(path, "");
            addName = "";
            addPath = "";
            ImGui::CloseCurrentPopup();
            ForceUpdateCache();
            SelectAsset(path);
        }

        if (ImGui::Button("Cancel##Asset", ImVec2(-1, 0)))
        {
            ImGui::CloseCurrentPopup();
            addName = "";
            addPath = "";
        }
        
        ImGui::EndPopup();
    }
}

#else

void AssetBrowser::Init() {}
void AssetBrowser::Deinit() {}
void AssetBrowser::DrawDebugPanel() {}

#endif
