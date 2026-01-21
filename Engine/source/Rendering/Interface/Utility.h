#pragma once
#include "Types.h"

namespace Rendering
{
    namespace WebGPU
    {
        int GetType(TextureType InType);
        int GetFormat(TextureFormat InFormat);
        void GetInternalFormats(TextureFormat InFormat, unsigned int *glInternalFormat, unsigned int *glFormat, unsigned int *glType);
        int GetParam(TextureParam InParam);
        int GetParamValue(TextureParamValue InValue);
        int GetAttachment(TextureAttachment InAttachment);
    }
}
