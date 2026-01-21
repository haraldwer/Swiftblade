#pragma once

#include "BakedFunc.h"
#include "Shader.h"
#include "Core/Property/Expressions/VectorExpression.h"
#include "Core/Resource/PropertyFile.h"

namespace Rendering
{
    struct ParticleData : PropertyOwner<ParticleData>
    {
        PROPERTY_D(ResShader, SimShader, "Shaders/Particles/SH_ParticleSim.ps");
        PROPERTY_D(int, MaxInstances, 200);
        PROPERTY_D(float, SystemLifetime, 5.0f);
        PROPERTY_D(int, BakeResolution, 128);

        PROPERTY_D(Expression, SpawnRate, "1");
        PROPERTY_D(Expression, Lifetime, "1");
        
        PROPERTY(ExprVec3, InitialPosition);
        PROPERTY(ExprVec3, InitialVelocity);
        PROPERTY(ExprVec3, Acceleration);
        PROPERTY(ExprVec3, Friction);
        PROPERTY_D(ExprVec4, Color, "1");
        PROPERTY_D(Expression, Scale, "1");
    };
    
    class Particle : public Resource::PropertyFile<ParticleData>
    {
        CLASS_INFO(Particle, Resource::PropertyFile<ParticleData>)

        friend class ParticlesRenderer;
        
    public:

        bool Load() override;
        bool Unload() override;
        bool Edit(const String& InName = "", uint32 InOffset = 0) override;
        static bool Accept(const String& InPath);

    private:
        
        template <class T>
        static Vec4F Eval(const ExprVec<T>& InExpr, float InAlpha)
        {
            InExpr.Set(0, InAlpha);
            return Vec4F(InExpr.Evaluate());
        }
        
        static Vec4F Eval(const Expression& InExprX, const Expression& InExprY, const Expression& InExprZ, float InAlpha)
        {
            InExprX.Set(0, InAlpha);
            InExprY.Set(0, InAlpha);
            InExprZ.Set(0, InAlpha);
            return {
                InExprX.Evaluate(),
                InExprY.Evaluate(),
                InExprZ.Evaluate(),
                0.0f
            };
        }
        
        // For easily accessing curves on the GPu
        void Bake();
        BakedFunc BakedExpressions;
        // 0 - InitialPosition
        // 1 - InitialVelocity
        // 2 - Acceleration
        // 3 - Friction
        // 4 - Color
        // 5 - Scale, SpawnRate, Lifetime
    };
}

typedef Resource::Ref<Rendering::Particle> ResParticle;