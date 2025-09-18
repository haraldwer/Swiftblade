#pragma once
#include "Container.h"

namespace UI
{
    enum class SplitDirection : uint8
    {
        HORIZONTAL = 0,
        VERTICAL
    };

    struct SplitProperties
    {
        float spacing = 0;
        SplitDirection direction = SplitDirection::HORIZONTAL;
        Vector<float> ratios;
    };
    
    class SplitContainer : public Container
    {
        CLASS_INFO(SplitContainer, Container);
        
    public:
        
        SplitContainer(const Transform& InTransform = Transform::Fill(), const SplitProperties& InProperties = {}) : Container(InTransform), properties(InProperties) {}
        void RefreshRect(Container &InOwner, const Rect &InContainingRect, bool InCacheVisible) override;

    private:
        SplitProperties properties;
    };
}
