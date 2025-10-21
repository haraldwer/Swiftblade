#pragma once

#include "BakedFunc.h"
#include "Shader.h"
#include "Property/Expressions/VectorExpression.h"
#include "Resource/PropertyFile.h"

namespace Rendering
{
    struct ParticleData : PropertyOwner<ParticleData>
    {
        PROPERTY_D(ResShader, SimShader, "Shaders/Particles/SH_ParticleSim.ps");
        PROPERTY_D(int, MaxInstances, 200);
        PROPERTY_D(float, SystemLifetime, 5.0f);
        PROPERTY_D(int, BakeResolution, 128);

        // All of these properties should be based on curves
        // And could change over time!
        
        PROPERTY(ExprVec3, InitialPosition);
        PROPERTY(ExprVec3, InitialVelocity);
        PROPERTY(ExprVec3, Acceleration);
        PROPERTY(ExprVec3, Friction);
        PROPERTY_D(ExprVec4, Color, "1");
        PROPERTY_D(Expression, Scale, "1");
        PROPERTY_D(Expression, SpawnRate, "1");
        PROPERTY_D(Expression, Lifetime, "1");
        
        // The default system should cover most cases
        // A system needs an instance, the resource only tells you how the instance should work
    };
    
    class Particle : public Resource::PropertyFile<ParticleData>
    {
        CLASS_INFO(Particle, Resource::PropertyFile<ParticleData>)

        friend class ParticlesRenderer;
        
    public:

        bool Load() override;
        bool Unload() override;
        bool Edit(const String& InName, uint32 InOffset) override;
        
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
    };
}

typedef Resource::Ref<Rendering::Particle> ResParticle;