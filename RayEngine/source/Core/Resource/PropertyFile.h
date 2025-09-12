#pragma once
#include "Base.h"
#include "File/File.h"

namespace Resource
{
    // Assume T is a propertyOwner?
    template<typename T>
    struct PropertyFile : Base
    {
        STRUCT_INFO(PropertyFile<T>, Base);
        
        bool Load() override
        {
            if (id.IsValid() && !id.Unique())
                return data.Load(id.Str());
            return false; 
        }
        
        bool Unload() override
        {
            data = T();
            return true;
        }
        
        bool Save() override
        {
            if (id.IsValid() && !id.Unique())
                return data.Save(id.Str());
            return false;
        }
        
        bool Edit(const String &InName, uint32 InOffset) override
        {
            return data.Edit(InName, InOffset);
        }

        Utility::Timepoint GetEditTime() const override
        {
            if (id.IsValid() && !id.Unique())
                return Utility::GetFileWriteTime(id.Str());
            return {};
        }

        T data;
    };
}