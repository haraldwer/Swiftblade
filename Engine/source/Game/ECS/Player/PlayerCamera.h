#pragma once
#include "Engine/ECS/UniqueComponent.h"
#include "PlayerInterface.h"

namespace ECS
{
    class PlayerCamera : public UniqueComponent<PlayerCamera>, public PlayerInterface
    {
    public:
        void Init() override;
        void Tick() override;
        int GetPriority() const override { return 8; }
        
        void AddCrouchOffset(float InHeightDiff);
        void SetTransform(const Mat4F& InTrans);
    
    private:

        PROPERTY_D(float, CrouchOffsetMultiplier, 0.5f);
        PROPERTY_D(float, CrouchInterpSpeed, 20.0f);
        
        float crouchOffset = 0.0f; 
        float targetCrouchOffset = 0.0f;
        
        Vec3F originalPosition;
    };
}
