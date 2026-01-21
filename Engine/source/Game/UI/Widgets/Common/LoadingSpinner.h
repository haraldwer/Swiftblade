#pragma once
#include "UI/Elements/Image.h"

namespace UI
{
    class LoadingSpinner : public Image
    {
        CLASS_INFO(LoadingSpinner, Image)
    public:
        LoadingSpinner(const Transform& InTrans = {
            .size = 50,
            .anchor = 0.5,
            .pivot = 0.5
        }) : Image(InTrans, {
            .texture = ResTexture("Defaults/T_Missing.png")
        }) {}
        void Update(Container &InOwner) override;
    };
}
