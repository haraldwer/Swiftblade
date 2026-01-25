#include "Model.h"
#include "tiny_obj_loader.h"
#include "Context/Context.h"

bool Rendering::ModelResource::Load()
{
    // Make reload safe
    Unload();

    // TODO: async model loading
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    bool result = reader.ParseFromFile(id.Str(), config);
    CHECK_RETURN_LOG(!result, "Failed to load model: " + id.Str(), false)

    const tinyobj::attrib_t& attrib = reader.GetAttrib();

    struct VertexLayout
    {
        float px, py, pz;
        float nx, ny, nz;
        float u, v;
    };

    struct VertexKey
    {
        int v = -1;
        int n = -1;
        int t = -1;

        bool operator==(const VertexKey& o) const noexcept
        {
            return v == o.v && n == o.n && t == o.t;
        }
    };

    struct VertexKeyHash
    {
        size_t operator()(const VertexKey& k) const noexcept
        {
            // Simple hash combine for 3 ints
            size_t h1 = std::hash<int>{}(k.v);
            size_t h2 = std::hash<int>{}(k.n);
            size_t h3 = std::hash<int>{}(k.t);
            size_t h = h1;
            h ^= h2 + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= h3 + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    auto UploadVertexBuffer = [&](const char* label, const void* data, uint64 sizeBytes) -> wgpu::Buffer
    {
        wgpu::BufferDescriptor bufferDesc;
        bufferDesc.label = wgpu::StringView(label);
        bufferDesc.size = sizeBytes;
        bufferDesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapWrite;
        bufferDesc.mappedAtCreation = true;

        wgpu::Buffer vb = Context::Get().CreateBuffer(bufferDesc);
        CHECK_ASSERT(!vb, "Failed to create vertex buffer");

        void* mapped = vb.getMappedRange(0, sizeBytes);
        CHECK_ASSERT(!mapped, "Failed to map vertex buffer");

        std::memcpy(mapped, data, static_cast<size_t>(sizeBytes));
        vb.unmap();
        return vb;
    };

    model.meshes.clear();
    model.lodMeshes.clear();
    model.meshes.reserve(reader.GetShapes().size());
    model.lodMeshes.reserve(reader.GetShapes().size());

    for (auto& shape : reader.GetShapes())
    {
        const auto& inMesh = shape.mesh;
        if (inMesh.indices.empty())
            continue;

        // Build indexed mesh (unique vertices + index buffer) from OBJ (v/n/uv are independently indexed)
        std::vector<VertexLayout> vertices;
        std::vector<uint32> indices;
        vertices.reserve(inMesh.indices.size());
        indices.reserve(inMesh.indices.size());

        std::unordered_map<VertexKey, uint32, VertexKeyHash> remap;
        remap.reserve(inMesh.indices.size());

        size_t indexOffset = 0;
        for (size_t f = 0; f < inMesh.num_face_vertices.size(); ++f)
        {
            const int fv = inMesh.num_face_vertices[f];
            // triangulate=true should make fv==3, but keep this generic.
            for (int vtx = 0; vtx < fv; ++vtx)
            {
                const tinyobj::index_t idx = inMesh.indices[indexOffset + static_cast<size_t>(vtx)];

                VertexKey key{ idx.vertex_index, idx.normal_index, idx.texcoord_index };

                auto it = remap.find(key);
                if (it != remap.end())
                {
                    indices.push_back(it->second);
                    continue;
                }

                VertexLayout v{};

                if (idx.vertex_index >= 0)
                {
                    const size_t vi = static_cast<size_t>(idx.vertex_index) * 3u;
                    v.px = attrib.vertices[vi + 0];
                    v.py = attrib.vertices[vi + 1];
                    v.pz = attrib.vertices[vi + 2];
                }

                if (idx.normal_index >= 0 && !attrib.normals.empty())
                {
                    const size_t ni = static_cast<size_t>(idx.normal_index) * 3u;
                    v.nx = attrib.normals[ni + 0];
                    v.ny = attrib.normals[ni + 1];
                    v.nz = attrib.normals[ni + 2];
                }
                else
                {
                    v.nx = 0.0f; v.ny = 0.0f; v.nz = 0.0f;
                }

                if (idx.texcoord_index >= 0 && !attrib.texcoords.empty())
                {
                    const size_t ti = static_cast<size_t>(idx.texcoord_index) * 2u;
                    v.u = attrib.texcoords[ti + 0];
                    v.v = attrib.texcoords[ti + 1];
                }
                else
                {
                    v.u = 0.0f; v.v = 0.0f;
                }

                const uint32 newIndex = static_cast<uint32>(vertices.size());
                vertices.push_back(v);
                remap.emplace(key, newIndex);
                indices.push_back(newIndex);
            }
            indexOffset += static_cast<size_t>(fv);
        }

        // Expand full-res to non-indexed triangles (since Mesh has only a vertex buffer)
        std::vector<VertexLayout> cpuVertices;
        cpuVertices.reserve(indices.size());
        for (uint32 i : indices)
            cpuVertices.push_back(vertices[i]);

        const uint64 fullSizeBytes = static_cast<uint64>(cpuVertices.size() * sizeof(VertexLayout));
        CHECK_RETURN_LOG(fullSizeBytes == 0, "Model has an empty mesh: " + id.Str(), false)

        wgpu::Buffer fullVB = UploadVertexBuffer("Mesh (full): " + id.Str(), cpuVertices.data(), fullSizeBytes);

        Mesh& outMesh = model.meshes.emplace_back();
        outMesh.vertexBuffer = fullVB;
        outMesh.vertexCount = static_cast<uint32>(cpuVertices.size());
        outMesh.vertexStride = sizeof(VertexLayout);

        // --- Generate one VERY low-poly LOD using meshoptimizer ---
        // Target: extremely aggressive reduction, but keep at least one triangle.
        const size_t indexCount = indices.size();
        size_t targetIndexCount = indexCount / 20; // ~5% triangles -> VERY low
        targetIndexCount = (targetIndexCount / 3) * 3; // must be multiple of 3
        if (targetIndexCount < 3) targetIndexCount = 3;
        if (targetIndexCount > indexCount) targetIndexCount = indexCount;

        std::vector<uint32> lodIndices(indexCount);
        const float targetError = 1e-1f; // allow large geometric error => fewer triangles
        const unsigned int options = 0;

        size_t lodIndexCount = meshopt_simplifySloppy(
            lodIndices.data(),
            indices.data(),
            indexCount,
            &vertices[0].px,              // position must be first 3 floats
            vertices.size(),
            sizeof(VertexLayout),
            targetIndexCount,
            targetError,
            options);

        if (lodIndexCount < 3)
            lodIndexCount = 3;

        lodIndices.resize(lodIndexCount);

        std::vector<VertexLayout> lodVerticesExpanded;
        lodVerticesExpanded.reserve(lodIndices.size());
        for (uint32 i : lodIndices)
            lodVerticesExpanded.push_back(vertices[i]);

        const uint64 lodSizeBytes = static_cast<uint64>(lodVerticesExpanded.size() * sizeof(VertexLayout));
        wgpu::Buffer lodVB = UploadVertexBuffer("Mesh (LOD0): " + id.Str(), lodVerticesExpanded.data(), lodSizeBytes);

        Mesh& outLod = model.lodMeshes.emplace_back();
        outLod.vertexBuffer = lodVB;
        outLod.vertexCount = static_cast<uint32>(lodVerticesExpanded.size());
        outLod.vertexStride = sizeof(VertexLayout);

        // Drop CPU mesh data ASAP (only GPU buffers remain)
        cpuVertices.clear();
        cpuVertices.shrink_to_fit();
        lodVerticesExpanded.clear();
        lodVerticesExpanded.shrink_to_fit();
        lodIndices.clear();
        lodIndices.shrink_to_fit();
        indices.clear();
        indices.shrink_to_fit();
        vertices.clear();
        vertices.shrink_to_fit();
    }

    // One LOD per mesh is guaranteed by push order above (we always push both or neither).
    CHECK_ASSERT(model.meshes.size() != model.lodMeshes.size(), "LOD generation mismatch: mesh/LOD counts differ");

    return true;
}

bool Rendering::ModelResource::Unload()
{
    
}

bool Rendering::ModelResource::Edit(const String &InName, uint32 InOffset)
{
    
}
