#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION

#include "MeshLoader.h"

#include "Context/Context.h"

#include "Tasks/Task.h"

void Rendering::ModelResource::LoadState() 
{
    state = {};
    state.primitiveState.topology = wgpu::PrimitiveTopology::TriangleList;
    state.primitiveState.frontFace = wgpu::FrontFace::CCW;
    state.primitiveState.cullMode = wgpu::CullMode::Back;
    state.primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
    
    // Create default layout
    auto& layout = state.vertexLayouts.emplace_back();
    static Vector<wgpu::VertexAttribute> attributes;
    if (attributes.empty())
    {
        // TODO: Make this cleaner
        attributes.resize(3);
        auto& positionAttr = attributes.at(0);
        positionAttr.format = wgpu::VertexFormat::Float32x3;
        positionAttr.offset = 0;
        positionAttr.shaderLocation = 0;
        auto& normalAttr = attributes.at(1);
        normalAttr.format = wgpu::VertexFormat::Float32x3;
        normalAttr.offset = 3 * sizeof(float);
        normalAttr.shaderLocation = 1;
        auto& uvAttr = attributes.at(2);
        uvAttr.format = wgpu::VertexFormat::Float32x2;
        uvAttr.offset = 2 * 3 * sizeof(float);
        uvAttr.shaderLocation = 2;
    }
    layout.attributes = attributes.data();
    layout.attributeCount = static_cast<uint32>(attributes.size());
    layout.arrayStride = sizeof(VertexLayout);
    layout.stepMode = wgpu::VertexStepMode::Vertex;
    
    // TODO: Instanced attributes
    
    struct HashData
    {
        uint32 prim = 0;
        uint32 attr = 0;
    } hashData;
    hashData.prim = Utility::Hash(state.primitiveState);
    hashData.attr = Utility::Hash(attributes);
    state.hash = Utility::Hash(hashData);
}

bool Rendering::ModelResource::Load()
{
    CHECK_ASSERT(!meshes.empty(), "Model already loaded");
    
    if (asyncLoadStarted)
        return true;
    
    asyncLoadStarted = true;
    loadCount++;
    LOG("Loading model: ", id.Str());
    Tasks::Enqueue<Resource::ID, ModelLoadResult>(id, &AsyncLoadModel);
    return true;
}

void Rendering::ModelResource::ContinueLoad()
{
    if (loadCount == 0)
        return;
    if (!asyncLoadStarted)
        return;
    
    Tasks::Consume<Resource::ID, ModelLoadResult>([&](const Resource::ID& InID, const ModelLoadResult& InResult)
    {
        CHECK_RETURN(InID != id, false);
        loadCount--;
        
        CHECK_RETURN_LOG(!InResult.success, "Failed to load model: " + id.Str(), false)
        CHECK_RETURN_LOG(!InResult.reader, "Reader invalid: " + id.Str(), false)

        LoadState();
        auto& shapes = InResult.reader->GetShapes();
        meshes.resize(shapes.size());
        
        for (int i = 0; i < shapes.size(); i++)
        {
            meshes.at(i).lods.resize(numGenLODs + 1);
            
            // Start new tasks
            loadCount++;
            MeshLoadParams params;
            params.reader = InResult.reader;
            params.id = id;
            params.shapeIndex = i;
            Tasks::Enqueue<MeshLoadParams, ObjectPtr<MeshData>>(params, &AsyncLoadMesh);
        }
        return true;
    }, 1);
    
    Tasks::Consume<MeshLoadParams, ObjectPtr<MeshData>>([&](const MeshLoadParams& InParams, const ObjectPtr<MeshData>& InData)
    {
        CHECK_RETURN(InParams.id != id.Str(), false);
        loadCount--;
        CHECK_RETURN_LOG(!InData, "Data invalid: " + InParams.id.Str(), false);
        
        if (InData->vertices.empty() || InData->indices.empty())
        {
            LOG("Failed to load shape: " + id.Str() + ":" + Utility::ToStr(InParams.shapeIndex));
            return true;
        }
        
        CHECK_ASSERT(InParams.shapeIndex >= meshes.size(), "Invalid shapeIndex")
        for (int lod = 0; lod <= numGenLODs; lod++)
        {
            loadCount++;
            LODLoadParams params { 
                id, 
                InParams.shapeIndex,
                lod,
                InData
            };
            Tasks::Enqueue<LODLoadParams, LODData>(params, &AsyncLoadLOD);
        }
        return true;
    }, 1);
    
    Tasks::Consume<LODLoadParams, LODData>([&](const LODLoadParams& InParams, const LODData& InResult)
    {
        CHECK_RETURN(InParams.id != id.Str(), false);
        loadCount--;
        
        CHECK_ASSERT(InParams.shapeIndex >= static_cast<int>(meshes.size()), "Invalid shapeIndex")
        auto& mesh = meshes.at(InParams.shapeIndex);
        CHECK_ASSERT(InParams.lodIndex >= static_cast<int>(mesh.lods.size()), "Invalid lodIndex")
        
        auto& vertices = InResult.vertices;
        auto& indices = InResult.indices;
        
        MeshLOD& lod = mesh.lods.at(InParams.lodIndex);
        lod.vertexCount = static_cast<uint32>(vertices.size());
        lod.indexCount = static_cast<uint32>(indices.size());
        lod.vertexStride = sizeof(VertexLayout);
        lod.vertexBuffer = Context::Get().UploadBuffer(
            "LOD" + Utility::ToStr(InParams.lodIndex + 1) + " VB: " + id.Str(),
            vertices.data(),
            vertices.size() * sizeof(VertexLayout),
            wgpu::BufferUsage::Vertex);

        bool canUseUint16 = (lod.vertexCount <= std::numeric_limits<uint16_t>::max());
        std::vector<uint16_t> indices16;
        if (canUseUint16)
        {
            indices16.reserve(indices.size());
            for (uint32 idx : indices)
            {
                if (idx > std::numeric_limits<uint16_t>::max())
                {
                    canUseUint16 = false;
                    break;
                }
                indices16.push_back(static_cast<uint16_t>(idx));
            }
        }
        
        const void* data = canUseUint16 ? static_cast<const void *>(indices16.data()) : static_cast<const void *>(indices.data());
        String ibType = canUseUint16 ? " IB16: " : " IB32: ";
        uint32 stride = canUseUint16 ? sizeof(uint16_t) : sizeof(uint32);
        size_t sizeBytes = (canUseUint16 ? indices16.size() : indices.size()) * stride;
        lod.indexFormat = canUseUint16 ? wgpu::IndexFormat::Uint16 : wgpu::IndexFormat::Uint32;
        lod.indexStride = stride;
        lod.indexBuffer = Context::Get().UploadBuffer(
            "LOD" + Utility::ToStr(InParams.lodIndex + 1 + 1) + ibType + id.Str(), 
            data, 
            sizeBytes,
            wgpu::BufferUsage::Index);
        
        return true;
    }, 1);
}

bool Rendering::ModelResource::Unload()
{
    if (!meshes.empty())
    {
        for (auto& mesh : meshes)
        {
            for (auto& lod : mesh.lods)
            {
                if (lod.vertexBuffer) lod.vertexBuffer.release();
                if (lod.indexBuffer) lod.indexBuffer.release();
            }
        }
        meshes.clear();
    }
    loadCount = 0;
    asyncLoadStarted = false;
    return true;
}

bool Rendering::ModelResource::Edit(const String &InName, uint32 InOffset)
{
    return false;
}

const Rendering::MeshState* Rendering::ModelResource::GetMeshState()
{
    if (state.hash > 0)
        return &state;
    ContinueLoad();
    return nullptr;
}

Rendering::MeshLOD const* Rendering::ModelResource::GetMesh(const int InMeshIndex, const int InLOD)
{
    ContinueLoad();
    if (InMeshIndex < 0 || InMeshIndex >= static_cast<int>(meshes.size()))
        return nullptr;
    auto& mesh = meshes.at(InMeshIndex); 
    const int lod = Utility::Math::Clamp(InLOD, 0, static_cast<int>(mesh.lods.size()) - 1);
    return &mesh.lods.at(lod);
}

int Rendering::ModelResource::GetMeshCount()
{
    ContinueLoad();
    return static_cast<int>(meshes.size());
}
