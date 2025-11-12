#pragma once

#include "Identifier.h"

namespace Resource
{
    struct Base
    {
        BASE_STRUCT_INFO(Base)

        Base() = default;
        virtual ~Base() {}

        Base(const ID& InID);
        virtual bool Load() = 0;
        virtual bool Unload() = 0;
        virtual bool Save() { return false; }
        virtual bool Edit(const String& InName, uint32 InOffset = 0) { return false; }
        
        virtual Utility::Timepoint GetEditTime() const
        {
            if (id.IsValid() && !id.Unique())
                return Utility::File::GetWriteTime(id.Str());
            return Utility::Timepoint();
        }
        
        ID id = {};
    };
}