#pragma once

#include "Core/Resource/Resource.h"

namespace Rendering
{
    struct AnimationData
    {
        // Optimized sample data
        struct Sample
        {
            int keyIndexFrom = 0;
            int keyIndexTo = 0;
        };
        
        enum class InterpType : uint8
        {
            LINEAR = 0,
            BEZIER,
            CONSTANT,
        };

        struct Keyframe
        {
            float frame = 0.0f;
            float value = 0.0f;
            InterpType interpolation = InterpType::LINEAR;
            Vec2F handle_left;
            Vec2F handle_right;
        };

        struct Curve
        {
            Vector<Keyframe> keyframes;
            Map<int, Sample> sampleCache;
        };
        
        typedef Map<int, Curve> Curves;
        typedef Map<String, Curves> Object;

        struct Action
        {
            String name;
            int frame;
        };
        
        String name;
        Vec2F frameRange;
        float duration_frames = 0.0f;
        float duration_seconds = 0.0f;
        float fps = 24.0f;
        Map<String, Object> objects;
        Vector<Action> actions;
    };
    
    class AnimationResource : Resource::Base
    {
        CLASS_INFO(AnimationResource, Resource::Base)

    public:
        bool Load() override;
        bool Unload() override;


        Mat4F Get(const String& InObject, float InTime);
        float GetCurve(const String& InObject, const String& InKey, float InTime, int InIndex = 0);
        float GetDuration() const { return data.duration_seconds; }
        
    private:
        Vec3F ReadVec3(AnimationData::Object& InObject, const String& InKey, float InTime);
        float ReadFloat(AnimationData::Object& InObject, const String& InKey, int InIndex, float InTime);
        float Read(AnimationData::Curves& InCurves, int InIndex, float InTime) const;
        AnimationData::Sample GetSampleData(AnimationData::Curve& InCurve, float InTime, float& OutFrame) const;

        AnimationData data;
    };
}

typedef Resource::Ref<Rendering::AnimationResource> ResAnim;