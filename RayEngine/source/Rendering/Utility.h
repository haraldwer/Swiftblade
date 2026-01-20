#pragma once

namespace Rendering
{
    inline Array<QuatF, 6> GetCubemapRotations()
    {
        return {
            QuatF::FromEuler({ 0, PI/2, PI/2 }), //Right
            QuatF::FromEuler({ 0, -PI/2, -PI/2 }), //Left
            QuatF::FromEuler({ PI/2, 0, PI }), //Up
            QuatF::FromEuler({ -PI/2, 0, PI }), //Down
            QuatF::FromEuler({ 0, 0, PI }), //Forward
            QuatF::FromEuler({ PI, 0, 0 }), //Backward
        };
    }
}