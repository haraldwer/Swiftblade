#include "VolumeDresser.h"

#include "CubeVolume.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Rendering/Debug/Draw.h"
#include "ImGui/imgui.h"

void SysVolumeDresser::Init(ECS::EntityID InID, VolumeDresser& InComponent)
{
    InComponent.Data = {};
    InComponent.Data.Load(InComponent.DressPath);
    Dress(InID); 
}

void SysVolumeDresser::Frame(ECS::EntityID InID, VolumeDresser& InComponent, double InDelta)
{
    auto& rs = Engine::Instance::Get().GetRenderScene(); 
    for (auto& mesh : InComponent.Instances)
        rs.AddMeshes(mesh.Mesh, mesh.Instances, mesh.MinBounds, mesh.MaxBounds);

    for (const Vec3F& missing : InComponent.DebugMissing)
        Rendering::DebugSphere(missing, 0.5f);
}

bool SysVolumeDresser::Edit(ECS::EntityID InID)
{
    auto& dresser = Get<VolumeDresser>(InID);
    const bool result = dresser.DressPath.Edit();
    if (dresser.DressPath.Get().empty())
        return result;

    if (ImGui::Button("Dress##DressData"))
        Dress(InID);
    ImGui::SameLine(); 
    if (ImGui::Button("Save##DressData"))
        if (dresser.Data.Save(dresser.DressPath))
            LOG("Dress data saved");
    
    static int selected = 0;  
    auto& data = dresser.Data.Data.Get();
    if (ImGui::BeginListBox("##DressData"))
    {
        for (int i = 0; i < data.size(); i++)
            if (ImGui::Selectable(data[i].Name.Get().c_str(), selected == i))
                selected = i;
        if (ImGui::Button("Add##DressData"))
            data.emplace_back();
        ImGui::EndListBox(); 
    }

    if (selected >= 0 && selected < data.size())
    {
        if (ImGui::Begin("Dress Editor"))
        {
            auto& entry = data[selected];
            entry.Name.Edit();
            entry.Model.Edit();
            entry.Material.Edit();
            auto& test = entry.Test.Get();

            if (ImGui::BeginTable("Dress Test", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody))
            {
                ImGui::TableSetupColumn("Y0", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("X0", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("X1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Y1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("X0", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("X1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(" ");
                ImGui::TableHeadersRow();
                
                const int min = 0;
                const int max = 1;
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Z0");

                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress0", ImGuiDataType_U8, &test[0], &min, &max);
                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress1", ImGuiDataType_U8, &test[1], &min, &max);
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress4", ImGuiDataType_U8, &test[4], &min, &max);
                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress5", ImGuiDataType_U8, &test[5], &min, &max);
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Z1");

                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress3", ImGuiDataType_U8, &test[3], &min, &max);
                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress2", ImGuiDataType_U8, &test[2], &min, &max);
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress7", ImGuiDataType_U8, &test[7], &min, &max);
                ImGui::TableNextColumn();
                ImGui::SliderScalar("##Dress6", ImGuiDataType_U8, &test[6], &min, &max);
            
                ImGui::EndTable(); 
            }
        }
        ImGui::End(); 
    }
    
    return result; 
}

void SysVolumeDresser::Dress(ECS::EntityID InID)
{
    LOG("Dressing volume")
    
    // Get components
    const ECS::Transform& trans = Get<ECS::Transform>(InID);
    const ECS::CubeVolume& volume = Get<ECS::CubeVolume>(InID);
    VolumeDresser& dresser = Get<VolumeDresser>(InID);
    const Vector<DressEntry>& dressData = dresser.Data.Data.Get();
    
    // Get base transform
    const Mat4F world = trans.World();
    
    // Iterate over volume
    // With an offset, check all
    dresser.DebugMissing.clear();

    struct TempDressEntry
    {
        Vector<Mat4F> Arr;
        Vec3F MinBounds = Vec3F(99999999.9f);
        Vec3F MaxBounds = Vec3F(-99999999.9f);
    };
    Map<uint8, TempDressEntry> pending;
    for (uint8 x = volume.CachedMinBounds.x; x <= volume.CachedMaxBounds.x + 1; x++)
    {
        for (uint8 y = volume.CachedMinBounds.y; y <= volume.CachedMaxBounds.y + 1; y++)
        {
            for (uint8 z = volume.CachedMinBounds.z; z <= volume.CachedMaxBounds.z + 1; z++)
            {
                // For each position
                // Evaluate neighbours
                const auto getVal = [&](const uint8 xOff, const uint8 yOff, const uint8 zOff)
                {
                    if ((x + xOff < 1) ||
                        (y + yOff < 1) ||
                        (z + zOff < 1))
                        return static_cast<uint8>(0);
                    const Coord pos = Coord(
                        x + xOff - 1,
                        y + yOff - 1,
                        z + zOff - 1);
                    const auto find = volume.Data.find(pos.Key);
                    if (find != volume.Data.end())
                        return static_cast<uint8>(find->second);
                    return static_cast<uint8>(0); 
                };

                // Gather neighbors 
                Array<uint8, 8> neighbors;
                neighbors[0] = getVal(0, 0, 0); 
                neighbors[1] = getVal(1, 0, 0); 
                neighbors[2] = getVal(1, 0, 1);
                neighbors[3] = getVal(0, 0, 1); 
                neighbors[4] = getVal(0, 1, 0); 
                neighbors[5] = getVal(1, 1, 0); 
                neighbors[6] = getVal(1, 1, 1);
                neighbors[7] = getVal(0, 1, 1); 

                uint8 rotation = 0;
                uint8 dress = 0;
                if (!EvaluateCoord(dressData, neighbors, dress, rotation))
                    continue;
                
                // Depending on neighbours, select config
                QuatF rot = QuatF::FromEuler(Vec3F(0.0f, PI_FLOAT * static_cast<float>(rotation) * 0.5f, 0.0f)); 
                Vec3F pos = volume.CoordToPos(Coord(x, y, z), world) - Vec3F::One();
                if (dress == static_cast<uint8>(-1))
                {
                    dresser.DebugMissing.push_back(pos);
                    continue;
                }

                auto& entry = pending[dress]; 
                entry.Arr.emplace_back(pos, rot, Vec3F(1.0f, 1.0f, 1.0f));

                // Update bounds
                entry.MinBounds.x = MIN(entry.MinBounds.x, pos.x);
                entry.MinBounds.y = MIN(entry.MinBounds.y, pos.y);
                entry.MinBounds.z = MIN(entry.MinBounds.z, pos.z);
                entry.MaxBounds.x = MAX(entry.MaxBounds.x, pos.x);
                entry.MaxBounds.y = MAX(entry.MaxBounds.y, pos.y);
                entry.MaxBounds.z = MAX(entry.MaxBounds.z, pos.z);
            }
        }
    }

    // Add instances 
    dresser.Instances.clear();
    int num = 0;
    for (auto& p : pending)
    {
        auto& dress = dressData[p.first];
        auto& data = dresser.Instances.emplace_back();
        data.Mesh.Model = dress.Model; 
        data.Mesh.Material = dress.Material;
        data.MinBounds = p.second.MinBounds - Vec3F::One();
        data.MaxBounds = p.second.MaxBounds + Vec3F::One();
        data.Instances = p.second.Arr;
        num += static_cast<int>(p.second.Arr.size());
    }
    LOG("Dressing complete, instances: " + std::to_string(num));
}

bool SysVolumeDresser::EvaluateCoord(const Vector<DressEntry>& InDressData, const Array<uint8, 8>& InNeighbors, uint8& OutDress, uint8& OutRotation) const
{
    const auto Evaluate = [&](const Array<uint8, 8>& InValid, uint8 InResultingDress) {
        
        const auto EvaluateRotation = [&](const uint8 InRotation) {
            
            // Rotate array index
            const auto RotationConvert = [&](const uint8 InIndex) {
                const uint8 rot = ((InIndex % 4) + InRotation) % 4;
                const uint8 vert = ((InIndex / 4) * 4);
                return static_cast<uint8>(rot + vert); 
            };

            // Compare array
            for (uint8 i = 0; i < 8; i++)
                if (InValid[i] != InNeighbors[RotationConvert(i)])
                    return false;
            return true; 
        };

        // Test every rotation 
        for (uint8 rot = 0; rot < 4; rot++)
        {
            if (EvaluateRotation(rot))
            {
                OutRotation = rot;
                OutDress = InResultingDress;
                LOG("Rotation: " + std::to_string(rot));
                return true; 
            }
        }
        return false; 
    };

    if (InNeighbors == Array<uint8, 8>({ 0, 0, 0, 0, 0, 0, 0, 0 }))
        return false; 

    for (uint8 i = 0; i < InDressData.size(); i++)
        if (Evaluate(InDressData[i].Test, i))
            return true;

    OutDress = -1; 
    return true; 
}
