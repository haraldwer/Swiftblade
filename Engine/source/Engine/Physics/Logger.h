#pragma once
#include "reactphysics3d/utils/Logger.h"

namespace Physics
{
    class Logger : public reactphysics3d::Logger
    {
        void log(Level level, const std::string &physicsWorldName, Category category, const std::string &message, const char *filename, int lineNumber) override
        {
            CHECK_RETURN(level >= Level::Information);
            String str;
            str += physicsWorldName + ":" + getLevelName(level) + " | ";
            str += "[" + getCategoryName(category) + "]: ";
            str += message;
            LOG(str);
        }
    };
}
