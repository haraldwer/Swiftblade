#include "Animation.h"

#include "Collections/SortedInsert.h"

bool Rendering::AnimationResource::Load()
{
    PROFILE();
    
    Unload();
    CHECK_RETURN_LOG(!Utility::FileExists(id.Str()), "File does not exist: " + id.Str(), false)
    const String fileContent = Utility::ReadFile(id.Str());
    CHECK_RETURN_LOG(fileContent.empty(), "Property file empty: " + id.Str(), false);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format: " + id.Str(), false);

    // Requires const, dont know why
    const rapidjson::Document& constDoc = doc;
    Utility::Deserialize(constDoc.GetObj(), "action_name", data.name);
    Utility::Deserialize(constDoc.GetObj(), "frame_range", data.frameRange);
    Utility::Deserialize(constDoc.GetObj(), "duration_frames", data.duration_frames);
    Utility::Deserialize(constDoc.GetObj(), "duration_seconds", data.duration_seconds);
    Utility::Deserialize(constDoc.GetObj(), "fps", data.fps);

    if (doc.HasMember("objects"))
    {
        for (auto itr = doc["objects"].MemberBegin(); itr != doc["objects"].MemberEnd(); ++itr)
        {
            String name;
            Utility::ReadValue(itr->name, name);
            auto& obj = data.objects[name];
            const auto& deObj = itr->value.GetObject();
            CHECK_CONTINUE(!deObj.HasMember("fcurves"));
            for (const auto& curveEntry : deObj["fcurves"].GetArray())
            {
                String dataPath;
                int arrayIndex = 0;
                Utility::Deserialize(curveEntry.GetObject(), "data_path", dataPath);
                Utility::Deserialize(curveEntry.GetObject(), "array_index", arrayIndex);
                auto& curve = obj[dataPath];
                auto& keyframes = curve[arrayIndex].keyframes;
                
                CHECK_CONTINUE(!curveEntry.HasMember("keyframes"));
                for (auto& keyframeEntry : curveEntry["keyframes"].GetArray())
                {
                    AnimationData::Keyframe keyframe;
                    Utility::Deserialize(keyframeEntry.GetObject(), "frame", keyframe.frame);
                    Utility::Deserialize(keyframeEntry.GetObject(), "value", keyframe.value);
                    int interp;
                    Utility::Deserialize(keyframeEntry.GetObject(), "interpolation", interp);
                    keyframe.interpolation = static_cast<AnimationData::InterpType>(interp);
                    Utility::Deserialize(keyframeEntry.GetObject(), "handle_left", keyframe.handle_left);
                    Utility::Deserialize(keyframeEntry.GetObject(), "handle_right", keyframe.handle_right);
                    Utility::SortedInsert(keyframes, keyframe, [](const AnimationData::Keyframe& a, const AnimationData::Keyframe& b)
                    {
                        return a.frame < b.frame;
                    });
                }
            }
        }
    }

    if (doc.HasMember("markers"))
    {
        for (const auto& markerEntry : doc["markers"].GetArray())
        {
            AnimationData::Action action;
            Utility::Deserialize(markerEntry.GetObject(), "name", action.name);
            Utility::Deserialize(markerEntry.GetObject(), "frame", action.frame);
            Utility::SortedInsert(data.actions, action, [](const AnimationData::Action& a, const AnimationData::Action& b)
            {
                return a.frame < b.frame;
            });
        }
    }

    return true;
}

bool Rendering::AnimationResource::Unload()
{
    data = {};
    return true;
}

Mat4F Rendering::AnimationResource::Get(const String &InObject, float InTime)
{
    PROFILE();
    auto find = data.objects.find(InObject);
    CHECK_RETURN(find == data.objects.end(), {})
    Vec3F loc = ReadVec3(find->second, "location", InTime);
    Vec3F rot = ReadVec3(find->second, "rotation_euler", InTime);
    Vec3F scale = ReadVec3(find->second, "scale", InTime);
    if (scale == Vec3F::Zero())
        scale = Vec3F::One();
    return Mat4F(loc, QuatF::FromEuler(rot), scale);
}

float Rendering::AnimationResource::GetCurve(const String& InObject, const String& InKey, const float InTime, const int InIndex)
{
    PROFILE();
    auto find = data.objects.find(InObject);
    CHECK_RETURN(find == data.objects.end(), {})
    return ReadFloat(find->second, InKey, InIndex, InTime);
}

Vec3F Rendering::AnimationResource::ReadVec3(AnimationData::Object &InObject, const String &InKey, const float InSample)
{
    auto find = InObject.find(InKey);
    CHECK_RETURN(find == InObject.end(), {})
    return {
        Read(find->second, 0, InSample),
        Read(find->second, 1, InSample),
        Read(find->second, 2, InSample)
    };
}

float Rendering::AnimationResource::ReadFloat(AnimationData::Object &InObject, const String &InKey, const int InIndex, const float InTime)
{
    auto find = InObject.find(InKey);
    CHECK_RETURN(find == InObject.end(), 0.0f)
    return Read(find->second, InIndex, InTime);
}

float Rendering::AnimationResource::Read(AnimationData::Curves& InCurves, const int InIndex, const float InTime) const
{
    const auto find = InCurves.find(InIndex);
    CHECK_RETURN(find == InCurves.end(), 0.0f)
    auto& curve = find->second;
    CHECK_RETURN(curve.keyframes.empty(), 0.0f)
    float frame = 0.0f;
    const auto sample = GetSampleData(curve, InTime, frame);
    
    // Assume sample data is correct
    const auto& from = curve.keyframes[sample.keyIndexFrom];
    const auto& to = curve.keyframes[sample.keyIndexTo];

    // TODO: More accurate lerp between frames?
    // Currently, alpha is bound to framerate
    
    switch (from.interpolation)
    {
        case AnimationData::InterpType::LINEAR:
        {
            // Now blend the current frame between these values
            const float alpha = (frame - to.frame) / (from.frame - to.frame);
            return Utility::Math::Lerp(from.value, to.value, alpha);
        }
        case AnimationData::InterpType::BEZIER:
        {
            const Vec2F k0 = { static_cast<float>(from.frame), from.value };
            const Vec2F k1 = { static_cast<float>(to.frame), to.value };
            const Vec2F h0 = from.handle_right;
            const Vec2F h1 = to.handle_left;
            return Utility::Math::InterpBezier(frame, k0, h0, h1, k1);
        }
    }
    
    return from.value;
}

Rendering::AnimationData::Sample Rendering::AnimationResource::GetSampleData(AnimationData::Curve& InCurve, const float InTime, float& OutFrame) const
{
    PROFILE();
    
    float loopTime = fmodf(InTime, data.duration_seconds); 
    float frame = loopTime * data.fps;
    OutFrame = Utility::Math::Clamp(frame, data.frameRange.r, data.frameRange.g);
    int floorFrame = static_cast<int>(OutFrame); // The actual frame we're at

    // Use cache and skip all calculations
    auto currFind = InCurve.sampleCache.find(floorFrame);
    if (currFind != InCurve.sampleCache.end())
        return currFind->second;

    // Use cache from previous frame to skip some calculations
    int loopStartIndex = 0;
    int prevFrame = Utility::Math::Max(0, floorFrame - 1); 
    auto prevFind = InCurve.sampleCache.find(prevFrame);
    if (prevFind != InCurve.sampleCache.end())
    {
        auto& prevTo = InCurve.keyframes.at(prevFind->second.keyIndexTo);
        if (prevTo.frame > static_cast<float>(floorFrame))
        {
            // This frame is still lerping to the same keyframe as in previous frame
            // Update cache to reflect this
            auto& cache = InCurve.sampleCache[floorFrame];
            cache = prevFind->second;
            return prevFind->second;
        }

        // Start the loop from keyframe of previous frame
        loopStartIndex = prevFind->second.keyIndexTo;
    }
    
    // We now have a bunch of keyframes!!
    // Now, find the correct keyframe...
    for (int i = loopStartIndex; i < static_cast<int>(InCurve.keyframes.size()); i++)
    {
        auto& keyframe = InCurve.keyframes.at(i);
        if (keyframe.frame > static_cast<float>(floorFrame))
        {
            AnimationData::Sample result;
            result.keyIndexTo = i;
            result.keyIndexFrom = Utility::Math::Max(0, i - 1);
            InCurve.sampleCache[floorFrame] = result;
            return result;
        }
    }

    // Assume last keyframe
    return {
        static_cast<int>(InCurve.keyframes.size()) - 1,
        static_cast<int>(InCurve.keyframes.size()) - 1
    };
}
