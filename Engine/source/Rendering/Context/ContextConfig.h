#pragma once

namespace Rendering
{
    struct ContextConfig : PropertyOwner<ContextConfig>
    {
        PROPERTY_D(String, CanvasID, "#canvas");
    };
}
