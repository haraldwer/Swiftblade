#pragma once
#include "Container.h"

namespace UI
{
    enum class SplitDirection : uint8
    {
        HORIZONTAL = 0,
        VERTICAL
    };
    
    class SplitContainer : public Container
    {
        TYPE_INFO(SplitContainer, Container);
        
    public:
        SplitContainer(const Transform& InTransform, const SplitDirection InDir = SplitDirection::HORIZONTAL, float InSpacing = 0.0f, const Vector<float>& InRatios = {}) : Container(InTransform), direction(InDir), ratios(InRatios), spacing(InSpacing) {}
        void RefreshRect(Container &InOwner, const Rect &InContainingRect) override;

    private:
        SplitDirection direction = SplitDirection::HORIZONTAL;
        Vector<float> ratios;
        float spacing = 0;
    };
}
