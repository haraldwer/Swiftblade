#include "AssetBrowser.h"

#include <filesystem>
#include <ranges>

#include "Utility/String/StringConversionTime.h"
#include "Collections/SortedInsert.h"
#include "ImGui/imgui.h"
#include "Resource/Impl.h"

#ifdef IMGUI_ENABLE

void AssetBrowser::Init()
{
    config.LoadConfig();
    config.Expanded.Get().insert(std::filesystem::current_path().string());
    TryStartThread();

    Resource::pickCallback = [](const String& InID)
    {
        Get().SelectAsset(InID);
    };

    Resource::dragDropCallback = [](String& OutResult)
    {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("assets"))
        {
            for (auto& s : Get().GetSelected())
            {
                OutResult = Utility::File::Relative(s);
                break;
            }
        }
    };
}

void AssetBrowser::Deinit()
{
    config.SaveConfig();
    TryStopThread();
    Resource::pickCallback = {};
    Resource::dragDropCallback = {};
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
                    Search(config.Search);
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
    PROFILE();
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
    {
        InNode.type = GetNodeType(InNode);
        InNode.size = static_cast<int>(file_size(InPath));
    }
    InNode.date = std::chrono::clock_cast<std::chrono::system_clock>(last_write_time(InPath));

    if (!InNode.dir)
        return;

    std::function<bool(const Node&, const Node&)> sortFunc = GetSortFunc();
    for (auto& path : std::filesystem::directory_iterator(InPath))
    {
        Node childNode;
        UpdateCache(path, childNode);
        Utility::SortedInsert(InNode.children, childNode, [&](const Node& InA, const Node& InB)
        {
            if (InA.dir == InB.dir)
                return sortFunc(InA, InB);
            return InA.dir;
        });
    }

    if (sortRev)
    {
        auto rev = std::ranges::views::reverse(InNode.children);
        InNode.children = { rev.begin(), rev.end() };
    }
}

void AssetBrowser::DrawPanel()
{
    PROFILE();
    
    ImGui::SetNextItemWidth(-1);
    String newSearch = config.Search;
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Search: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##AssetSearch", &newSearch, ImGuiInputTextFlags_AutoSelectAll);
    if (newSearch != config.Search.Get())
        Search(newSearch);
    
    if (ImGui::BeginTable("Assets", 4, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        // Type?

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
        header("Type", Sort::TYPE);
        ImGui::TableSetColumnIndex(2);
        header("Size", Sort::SIZE);
        ImGui::TableSetColumnIndex(3);
        header("Date", Sort::DATE);

        rootLock.lock();
        editLocked = true;
        for (auto& child : root.children)
            DrawNode(child);
        rootLock.unlock();
        editLocked = false;
        ImGui::EndTable();
    }

    DrawAddMenu();
}

void AssetBrowser::SelectAsset(const String &InPath)
{
    String relative = Utility::File::Relative(InPath);
    std::function<bool(const Node&)> select = [&](const Node& InNode)
    {
        if (Utility::File::Relative(InNode.path.string()) == relative)
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

Set<String> AssetBrowser::GetSelected()
{
    return config.Selected.Get();
}

String AssetBrowser::GetNodeType(const Node& InNode)
{
    if (InNode.ext == ".json")
    {
        if (InNode.name.starts_with("RM_"))
            return "Render Mat";
        if (InNode.name.starts_with("PM_"))
            return "Phys Mat";
        if (InNode.name.starts_with("PS_"))
            return "Particle";
        if (InNode.name.starts_with("BP_"))
            return "Blueprint";
        if (InNode.name.starts_with("C_"))
            return "Config";
        if (InNode.name.starts_with("BT_"))
            return "Baked Tex";
        if (InNode.name.starts_with("T_"))
            return "Proc Tex";
        if (InNode.name.starts_with("S_"))
            return "Scene";
        return "Json";
    }

    if (InNode.ext == ".txt") return "Text";
    if (InNode.ext == ".ini") return "Conf";
    if (InNode.ext == ".png") return "Image";
    if (InNode.ext == ".obj") return "Model";
    if (InNode.ext == ".ttf") return "Font";
    
    if (InNode.name.starts_with("SH_"))
        return "Shader";

    return "Unknown";
}

std::function<bool(const AssetBrowser::Node&, const AssetBrowser::Node&)> AssetBrowser::GetSortFunc() const
{
    switch (sort)
    {
    case Sort::NAME: return [](const Node& InA, const Node& InB) { return InA.name.compare(InB.name) < 0; };
    case Sort::DATE: return [](const Node& InA, const Node& InB) { return InA.date < InB.date; };
    case Sort::SIZE: return [](const Node& InA, const Node& InB) { return InA.size < InB.size; };
    case Sort::TYPE: return [](const Node& InA, const Node& InB) { return InA.type.compare(InB.type) < 0; };
    }
    return {};
}

void AssetBrowser::SelectNode(const Node& InNode)
{
    PROFILE();
    
    bool singleSelect = !ImGui::IsKeyDown(ImGuiKey_LeftShift);
    bool alreadySelected = config.Selected.Get().contains(InNode.path.string());
    bool openDirectly = config.Selected.Get().empty();
    
    if (singleSelect)
        config.Selected.Get().clear();
    config.Selected.Get().insert(InNode.path.string());
    LOG("Selection: " + InNode.name);

    if (InNode.dir)
    {
        if (singleSelect)
        {
            if (!config.Expanded.Get().contains(InNode.path.string()) || !alreadySelected)
                config.Expanded.Get().insert(InNode.path.string());
            else config.Expanded.Get().erase(InNode.path.string());
        }
    }
    else if (alreadySelected || openDirectly)
    {
        if (singleSelect)
            details.Clear();
        details.Select({ Utility::File::Relative(InNode.path.string()) });
    }
}

void AssetBrowser::NodeDragDrop(const Node &InNode)
{
    // Drag drop
    if (ImGui::BeginDragDropSource())
    {
        auto& selected = config.Selected.Get();
        if (!selected.contains(InNode.path.string()))
            SelectNode(InNode);
        
        if (ImGui::GetDragDropPayload() == NULL)
        {
            AssetBrowser* ptr = this;
            ImGui::SetDragDropPayload("assets", &ptr, sizeof(AssetBrowser*));
            LOG("BeginDragDrop on " + InNode.name)
        }

        if (selected.size() == 1)
            ImGui::Text(Utility::File::Name(*selected.begin()).c_str());
        else ImGui::Text("%i assets", static_cast<int>(selected.size()));
        
        ImGui::EndDragDropSource();
    }
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("assets"))
        {
            AssetBrowser* ptr = *static_cast<AssetBrowser**>(p->Data);
            if (ptr == this)
            {
                LOG("EndDragDrop on " + InNode.name)
                
                // Accept drop
                std::filesystem::path dir = InNode.dir ?
                    InNode.path : InNode.path.parent_path();

                for (auto& s : config.Selected.Get())
                {
                    // Copy and remove
                    std::filesystem::path source = s;
                    String filename = Utility::File::Name(s);
                    std::filesystem::path target = dir.string() + "/" + filename;
                    if (Utility::File::Copy(source.string(), target.string(), true))
                        Utility::File::Delete(source.string());
                }
                ForceUpdateCache();
            }
        }
        
        ImGui::EndDragDropTarget();
    }
}

void AssetBrowser::NodeContext(const Node &InNode)
{
    PROFILE();
    
    if (ImGui::BeginPopupContextItem(InNode.path.string().c_str()))
    {
        if (!config.Selected.Get().contains(InNode.path.string()))
            SelectNode(InNode);
        
        if (config.Selected.Get().size() <= 1) ImGui::Text(InNode.name.c_str());
        else ImGui::Text((Utility::ToStr(config.Selected.Get().size()) + " assets").c_str());

        if (ImGui::Selectable("Open##Popup"))
        {
            details.Clear();
            for (auto& s : config.Selected.Get())
                if (!std::filesystem::is_directory(s))
                    details.Select({ Utility::File::Relative(s) });
        }

        if (ImGui::Selectable("New file##Popup"))
        {
            if (InNode.dir)
            {
                addPath = InNode.path.string();
                addName = "";
            }
            else
            {
                addPath = InNode.path.parent_path().string();
                addName = InNode.name + InNode.ext;
            }
            
            openAdd = true;
        }

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
        for (auto& s : config.Selected.Get())
            if (!std::filesystem::is_directory(s))
                details.Select({ Utility::File::Relative(s) });
}

void AssetBrowser::BeginCut()
{
    LOG("Begin cut");
    String cutCmd = "CUT:";
    for (auto& s : config.Selected.Get())
        cutCmd += s + ";";
    ImGui::SetClipboardText(cutCmd.c_str());
}

void AssetBrowser::BeginCopy()
{
    LOG("Begin copy");
    String copyCmd = "CPY:";
    for (auto& s : config.Selected.Get())
        copyCmd += s + ";";
    ImGui::SetClipboardText(copyCmd.c_str());
}

void AssetBrowser::Paste()
{
    String clip = ImGui::GetClipboardText();
    if (clip.starts_with("CUT:") || clip.starts_with("CPY:"))
    {
        Set<std::filesystem::path> targetDirs;
        if (config.Selected.Get().empty())
            return;
        for (auto& s : config.Selected.Get())
            if (std::filesystem::is_directory(s)) targetDirs.insert(s);
            else targetDirs.insert(std::filesystem::path(s).parent_path());
        
        String content = clip.substr(4);
        Utility::StringSplit(content, ";", [&](const String& sourcePath)
        {
            String name = Utility::File::Name(sourcePath);
            for (auto& t : targetDirs)
            {
                String targetPath = t.string() + "/" + name;
                Utility::File::Copy(sourcePath, targetPath, true);
            }

            if (clip.starts_with("CUT:"))
                Utility::File::Delete(sourcePath);
        });

        ForceUpdateCache();
        ImGui::SetClipboardText("");
    }
}

void AssetBrowser::Duplicate()
{
    for (auto& s : config.Selected.Get())
        Utility::File::Copy(s, s, true);
    ForceUpdateCache();
}

void AssetBrowser::Delete()
{
    for (auto& s : config.Selected.Get())
        Utility::File::Delete(s);
    ForceUpdateCache();
}

void AssetBrowser::DrawNode(Node& InNode)
{
    PROFILE();
    
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    bool found = searchResult.empty() || searchResult.contains(InNode.path);
    if (!found)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128 ,255));

    bool select = config.Selected.Get().contains(InNode.path.string());
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
    ImGui::Text(Utility::ToStr(InNode.type).c_str());
    ImGui::TableNextColumn();
    ImGui::Text(Utility::ToStr(InNode.size).c_str());
    ImGui::TableNextColumn();
    ImGui::Text(Utility::ToStr(InNode.date).c_str());
    ImGui::PopStyleColor();

    if (InNode.dir && config.Expanded.Get().contains(InNode.path.string()))
    {
        ImGui::Indent();
        for (auto& child : InNode.children)
            DrawNode(child);
        ImGui::Unindent();
    }
}

void AssetBrowser::Search(const String& InStr)
{
    config.Search = InStr;
    searchSplit = Utility::StringSplit(InStr, " ");
    searchResult.clear();
    if (!searchSplit.empty())
        config.Expanded.Get().clear();

    std::function<bool(const Node&)> func;
    func = [&](const Node& InNode)
    {
        bool result = false;
        for (const auto& s : searchSplit)
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
            config.Expanded.Get().insert(InNode.path.string());
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
        ImGui::Text("Path: %s", Utility::File::Relative(path).c_str());
        
        if (addName.empty())
        {
            ImGui::Text("Error: No name specified");
        }
        else if (Utility::File::Exists(path))
        {
            ImGui::Text("Error: File already exists");
        }
        else if (ImGui::Button("Create##Asset", ImVec2(-1, 0)))
        {
            Utility::File::Write(path, "");
            addName = "";
            addPath = "";
            ImGui::CloseCurrentPopup();
            ForceUpdateCache();
            SelectAsset(path); // TODO: Force open
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
