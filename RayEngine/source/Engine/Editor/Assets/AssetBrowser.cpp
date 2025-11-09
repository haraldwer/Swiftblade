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
    running = true;
    thread = std::thread([&]()
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
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
}

void AssetBrowser::Deinit()
{
    running = false;
    thread.join();
}

void AssetBrowser::DrawDebugPanel()
{
    ImGui::SetNextItemWidth(-1);
    String newSearch = search;
    ImGui::InputText("##AssetSearch", &newSearch, ImGuiInputTextFlags_AutoSelectAll);
    if (newSearch != search)
        Search(newSearch);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 7));
    if (ImGui::BeginChild("AssetPanel", ImVec2(-1, -1), ImGuiChildFlags_AlwaysUseWindowPadding))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(7, 0));
        if (ImGui::BeginTable("Assets", 3, ImGuiTableFlags_ScrollY))
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
                    rootLock.lock();
                    Node newRoot;
                    UpdateCache(std::filesystem::current_path(), newRoot);
                    root = newRoot;
                    rootLock.unlock();
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
            DrawNode(root);
            rootLock.unlock();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
    ImGui::PopStyleVar();
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
    
    if (InNode.dir && singleSelect)
    {
        if (expanded.contains(InNode.path))
            expanded.erase(InNode.path);
        else
            expanded.insert(InNode.path);
        return;
    }
    
    if (singleSelect)
        ClearSelection();

    if (selected.contains(InNode.path))
    {
        selected.erase(InNode.path);
        details.Deselect(InNode.path);
    }
    else
    {
        selected.insert(InNode.path);
        details.Select({ InNode.path.string() });
    }
}

void AssetBrowser::ClearSelection()
{
    selected.clear();
    details.Clear();
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
    // Context menu
    if (ImGui::BeginPopupContextItem(InNode.name.c_str()))
    {
        ImGui::SeparatorText(InNode.name.c_str());        
        if (ImGui::Selectable("Select##Popup"))
            SelectNode(InNode);

        if (InNode.dir)
        {
            const bool exp = expanded.contains(InNode.path);
            if (ImGui::Selectable(exp ? "Collapse##Popup" : "Expand##Popup"))
                if (exp) expanded.erase(InNode.path);
                else expanded.insert(InNode.path);
        }

        ImGui::Separator();

        if (ImGui::Selectable("Cut##Popup")) {}
        if (ImGui::Selectable("Copy##Popup")) {}
        if (ImGui::Selectable("Duplicate##Popup")) {}
        if (ImGui::Selectable("Delete##Popup")) {}
        
        ImGui::Separator();


        ImGui::EndPopup();
    }
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

void AssetBrowser::DrawNode(Node& InNode)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    bool found = searchResult.empty() || searchResult.contains(InNode.path);
    if (!found)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128 ,255));

    bool select = selected.contains(InNode.path);
    float height = ImGui::CalcTextSize("T").y + 2;
    if (ImGui::Selectable(InNode.name.c_str(), select, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, height)))
        SelectNode(InNode);
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

#else

void AssetBrowser::Init() {}
void AssetBrowser::Deinit() {}
void AssetBrowser::DrawDebugPanel() {}

#endif
