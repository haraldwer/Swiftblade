#include "RoomGenEditor.h"

#include "Engine/Rendering/Debug/Draw.h"
#include "Game/ECS/RoomConnection.h"
#include "Game/Editor/RoomSubEditorManager.h"
#include "Utility/Hash.h"
#include "Utility/RandomWeightedCollection.h"

void RoomGenEditor::Init()
{
    // Create UI
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(Vec2F(80.0f, 80.0f), 0.0, 0.5f)))
            .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
            .Add(UI::Image(ResTexture("UI/T_Exit.png"), UI::Transform::SimpleAnchor(0.5f)), "Loading");
    UI = builder.Build();
}

void RoomGenEditor::Deinit()
{
    StartEntity = ECS::InvalidID;
    EndEntity = ECS::InvalidID;
}

void RoomGenEditor::Enter()
{
    CurrentState = GenerationStage::PATH;
        
    // Find start and end
    auto& sys = ECS::Manager::Get().GetSystem<SysRoomConnection>();
    for (const ECS::EntityID connection : sys.GetEntities())
    {
        if (sys.Get<RoomConnection>(connection).IsEnd)
            EndEntity = connection;
        else
            StartEntity = connection;
    }

    // Clear current volume
    auto& v = GetVolume();
    v.Data.clear();
    v.UpdateCache(Mat4F());

    // Clear gen data
    Path.clear();
    PathSet.clear();
    QueuedCoords.clear();
    NextQueue.clear();
    CheckedCoords.clear();
    Result.clear();
    VolumeDepth = 0;
    
    // Maybe clear spawned objects too?
}

void RoomGenEditor::GeneratePath()
{
    Coord coord = 0;
    Coord start = 0;
    
    if (!Path.empty())
        coord = Path.back();
    
    auto& v = GetVolume();
    if (auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(StartEntity))
    {
        start = v.PosToCoord(t->GetPosition() + Vec3F(1, 1, -1) * 0.6f);
        if (coord.Key == 0)
            coord = start;
    }
    
    Coord target = 0;
    if (auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(EndEntity))
    {
        target = v.PosToCoord(t->GetPosition() + Vec3F(1, 1, -1) * 0.6f);
    }
    
    if (coord.Key == target.Key)
    {
        CurrentState = GenerationStage::VOLUME;
        return;
    }
    
    // There are a few potential directions
    // Any direction could work actually
    // Add to a pool

    const int seed = static_cast<int>(Utility::Hash(Utility::Time::Get().Total()));
    Utility::RandomWeightedCollection<Coord> pool(seed);

    Vec3F startPos = Vec3F(start.Pos.X, start.Pos.Y, start.Pos.Z);
    Vec3F targetPos = Vec3F(target.Pos.X, target.Pos.Y, target.Pos.Z);
    Vec3F basePos = Vec3F(coord.Pos.X, coord.Pos.Y, coord.Pos.Z);
    
    Vec3F lastDir = Vec3F::Forward();
    if (Path.size() > 1)
    {
        Coord lastCoord = Path[Path.size() - 2];
        Vec3F lastPos = Vec3F(lastCoord.Pos.X, lastCoord.Pos.Y, lastCoord.Pos.Z);
        lastDir = (basePos - lastPos).GetNormalized();
    }
    
    auto evaluateDirection = [&](const Vec3I& InDirection)
    {
        const int X = InDirection.x + coord.Pos.X;
        const int Y = InDirection.y + coord.Pos.Y;
        const int Z = InDirection.z + coord.Pos.Z;
        if (X < 0 ||
            Y < 0 ||
            Z < 0 ||
            X >= static_cast<uint8>(-1) ||
            Y >= static_cast<uint8>(-1) ||
            Z >= static_cast<uint8>(-1))
            return;

        Coord c = Coord(
            static_cast<uint8>(X),
            static_cast<uint8>(Y),
            static_cast<uint8>(Z));
        if (PathSet.contains(c.Key))
            return;
        
        // Distance
        Vec3F coordPos = Vec3F(c.Pos.X, c.Pos.Y, c.Pos.Z);
        float dist = (targetPos - coordPos).Length(); 
        float totalDist = (targetPos - startPos).Length();
        float distPart = Utility::Math::Min(1.0f, dist / totalDist);

        // Dot 
        Vec3F dir = (coordPos - basePos).GetNormalized();
        Vec3F targetDir = (targetPos - coordPos).GetNormalized();
        float dot = dir.Dot(targetDir);

        // Last dot
        float lastDot = Utility::Math::Lerp(1.0f, lastDir.Dot(dir), 0.7f);
        
        // Stupidness, 
        float stupidness = 0.6f; // Stupidness factor

        // Only change direction ever so often...
        float weight = (dot + stupidness * distPart) * lastDot;
        if (weight < 0.001f)
            return;
        
        pool.Add(c, weight);
    };
    
    // Add every direction
    evaluateDirection({ 1, 0, 0 });
    evaluateDirection({ -1, 0, 0 });
    evaluateDirection({ 0, 1, 0 });
    evaluateDirection({ 0, -1, 0 });
    evaluateDirection({ 0, 0, 1 });
    evaluateDirection({ 0, 0, -1 });

    if (pool.Count() == 0)
    {
        Path.push_back(target);
        PathSet.insert(target.Key);
        CurrentState = GenerationStage::VOLUME;
        return;
    }
    
    Coord result = pool.Pop();
    CHECK_RETURN(PathSet.contains(result.Key));
    Path.push_back(result);
    PathSet.insert(result.Key);
    if (result.Key == target.Key)
        CurrentState = GenerationStage::VOLUME;
    
}

void RoomGenEditor::GenerateVolume()
{
    // There is a path
    // The path "grows"

    if (QueuedCoords.empty())
    {
        CheckedCoords.clear();
        Result.clear();
        NextQueue.clear();
        VolumeDepth = 0;
        for (const uint32 c : PathSet)
            QueuedCoords.push_back({c, c}); 
    }

    auto& v = GetVolume();
    
    auto tryQueueEntry = [&](Coord InBase, Coord InReference, const Vec3I& InDirection)
    {
        const int X = InDirection.x + InBase.Pos.X;
        const int Y = InDirection.y + InBase.Pos.Y;
        const int Z = InDirection.z + InBase.Pos.Z;
        if (X < 0 ||
            Y < 0 ||
            Z < 0 ||
            X >= static_cast<uint8>(-1) ||
            Y >= static_cast<uint8>(-1) ||
            Z >= static_cast<uint8>(-1))
            return;
        const Coord c = Coord(
            static_cast<uint8>(X),
            static_cast<uint8>(Y),
            static_cast<uint8>(Z));
        if (CheckedCoords[InReference.Key].contains(c.Key))
            return;
        CheckedCoords[InReference.Key].insert(c.Key); 
        NextQueue[c.Key] = InReference.Key;
    };
    
    auto evaluateCoord = [&](Coord InCoord, Coord InReference, uint8& InOutValue) -> bool
    {
        // Start and end
        Coord front = Path.front();
        Coord back = Path.back();
        if (InCoord.Pos.Z <= front.Pos.Z ||
            InCoord.Pos.Z >= back.Pos.Z)
        {
            if (InCoord.Pos.Z == front.Pos.Z || InCoord.Pos.Z == back.Pos.Z)
            {
                Coord comp = InCoord.Pos.Z == front.Pos.Z ? front : back;
                if (InCoord.Pos.X - comp.Pos.X > 0 ||
                    InCoord.Pos.X - comp.Pos.X < -1 ||
                    InCoord.Pos.Y - comp.Pos.Y > 2 ||
                    InCoord.Pos.Y - comp.Pos.Y < 0)
                {
                    InOutValue = 1; // Wall
                    return false;
                }
                return true; // Continue, to get ground below entrance
            }
            return false;
        }

        InOutValue = 1;
        if (Result.contains(InCoord.Key))
            InOutValue = Result[InCoord.Key];
        
        // Entrance and exit ground cap
        bool checkHeight = true;
        if (abs(InCoord.Pos.X - front.Pos.X) < 1 &&
            abs(InCoord.Pos.Z - front.Pos.Z) < 1)
            if (InCoord.Pos.Y >= front.Pos.Y)
                checkHeight = false;
        if (abs(InCoord.Pos.X - back.Pos.X) < 1 &&
            abs(InCoord.Pos.Z - back.Pos.Z) < 1)
            if (InCoord.Pos.Y >= back.Pos.Y)
                checkHeight = false;
        
        // Height
        if (checkHeight)
        {
            const int height = InCoord.Pos.Y - InReference.Pos.Y;
            if (height > 7 || height < 0)
                return false;
        }

        // Distance
        if (!(abs(InCoord.Pos.X - InReference.Pos.X) < 7 &&
            abs(InCoord.Pos.Z - InReference.Pos.Z) < 7))
            return false;

        InOutValue = 0;
        return true;
    };

    for (int i = 0; i < 100; i++)
    {
        if (QueuedCoords.empty())
            break;
        QueuedEntry entry = QueuedCoords.front();
        QueuedCoords.erase(QueuedCoords.begin());
        
        uint8 val = 0;
        bool success = evaluateCoord(entry.Coord, entry.Ref, val);

        uint8& existingValue = Result[entry.Coord];
        if (existingValue != val)
        {
            existingValue = val;
            if (val > 0)
                v.Data[entry.Coord] = val;
            else if (v.Data.contains(entry.Coord))
                v.Data.erase(entry.Coord);
        }

        if (!success)
            continue;
        
        tryQueueEntry(entry.Coord, entry.Ref, { 1, 0, 0 });
        tryQueueEntry(entry.Coord, entry.Ref, { -1, 0, 0 });
        tryQueueEntry(entry.Coord, entry.Ref, { 0, 1, 0 });
        tryQueueEntry(entry.Coord, entry.Ref, { 0, -1, 0 });
        tryQueueEntry(entry.Coord, entry.Ref, { 0, 0, 1 });
        tryQueueEntry(entry.Coord, entry.Ref, { 0, 0, -1 });
    }

    if (QueuedCoords.empty())
    {
        for (auto& q : NextQueue)
            QueuedCoords.push_back({q.first, q.second});
        NextQueue.clear();
        
        VolumeDepth++;
        v.UpdateCache(Mat4F());
    }

    if (QueuedCoords.empty())
    {
        v.Data.clear();
        for (auto& val : Result)
            if (val.second > 0)
                v.Data[val.first] = val.second;
        v.UpdateCache(Mat4F());
        CurrentState = GenerationStage::PROPPING;
        LOG("Finished. Depth: " + std::to_string(VolumeDepth));
    }
}

void RoomGenEditor::GenerateProps()
{
}

void RoomGenEditor::Update()
{
    switch (CurrentState) {
    case GenerationStage::PATH:
        GeneratePath();
        break;
    case GenerationStage::VOLUME:
        GenerateVolume();
        break;
    case GenerationStage::PROPPING:
        GenerateProps();
        break;
    case GenerationStage::FINISHED:
        Owner->SetMode(SubEditorMode::OBJECTS);
        break;
    }

    for (auto c : Path)
        Rendering::DebugSphere(GetVolume().CoordToPos(c));
}

void RoomGenEditor::Frame(bool InIsCameraControlling)
{
    if (UI)
    {
        //UI::Image& image = UI->Get<UI::Image>("Loading");
        //UI::Transform trans = image.GetTransform();
        //trans.Rotation = Utility::Time::Get().Total();
        //image.SetTransform(trans);
        
        //UI->Draw();
    }
}
