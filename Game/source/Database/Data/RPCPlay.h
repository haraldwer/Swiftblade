#pragma once

namespace DB
{
    struct BeginPlayData
    {
        String levelID;
        String levelHash;
        int seed;
    };

    struct EndPlayData
    {
        String levelID;
        String levelHash;
        int seed;
        int score;
    };
}
