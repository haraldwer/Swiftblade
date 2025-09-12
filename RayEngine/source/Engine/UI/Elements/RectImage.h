#pragma once
#include "Image.h"

namespace UI
{
    class RectImage : public Image
    {
        CLASS_INFO(RectImage, Image)

    public:
        
        RectImage(const Transform& InTransform = Transform::Fill(), ImageProperties InProperties = {}, Margin InEdges = {}) : Image(InTransform, InProperties), edges(InEdges) {}
        void Draw(Container& InOwner) override;

    private:
        Margin edges; 
    };
}
