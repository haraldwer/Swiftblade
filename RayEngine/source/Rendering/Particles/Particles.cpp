#include "Particles.h"

#include "ParticlesRenderer.h"
#include "raylib.h"
#include "Scene/Scene.h"

void Rendering::Particles::Init(const ContextConfig& InConfig)
{
    
}

void Rendering::Particles::Deinit()
{
    persistence = {};
    for (auto& p : particles)
        for (auto& target : p.second.All())
            target.Unload();
    particles.clear();
}

Rendering::Pipeline::Stats Rendering::Particles::Update(const RenderArgs& InArgs)
{
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene")

    Pipeline::Stats stats;
    persistence.Begin();

    // TODO: Time splitting
    for (auto& instance : InArgs.scenePtr->particles.GetAll())
    {
        CHECK_CONTINUE(instance.id == 0);
        persistence.Touch(instance.id);
        auto& target = particles[instance.id];
        if (TryCreate(target, instance))
            stats.particleSims += ParticlesRenderer::Simulate(InArgs, target, instance);
    }

    for (auto& rem : persistence.GetRemoved())
    {
        auto& p = particles.at(rem);
        for (auto& target : p.All())
            target.Unload();
        particles.erase(rem);
    }
    return stats;
}

Vector<Rendering::ParticleInstanceData> Rendering::Particles::GetFrameInstances(const RenderArgs& InArgs)
{
    Vector<ParticleInstanceData> data;
    auto culled = InArgs.scenePtr->particles.GetCulled(InArgs.cullPoints);
    for (auto& instance : culled)
    {
        CHECK_CONTINUE(instance.id == 0);
        auto& p = particles[instance.id];
        if (p.Curr().IsValid())
            data.push_back({
                instance,
                &p.Curr()
            });
    }
    return data;
}

bool Rendering::Particles::TryCreate(SwapTarget& InTarget, const ParticleInstance& InValue)
{
    CHECK_RETURN(InTarget.Curr().IsValid(), true);
    
    // Calculate resolution
    auto particle = InValue.particle.Get();
    CHECK_RETURN(!particle, false);

    int sqrt = static_cast<int>(ceilf(sqrtf(static_cast<float>(particle->data.MaxInstances.Get()))));
    int res = Utility::Math::NextPow2(sqrt);

    for (auto& t : InTarget.All())
    {
        if (t.TryBeginSetup(res))
        {
            t.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.CreateBuffer("TexVelocity", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.EndSetup();
        }
    }
    return true;
}
