#pragma once

#include "Identifier.h"
#include "Base.h"

namespace Resource
{
    // Editing
    String Pick(const String& InLabel, const String& InID);
    bool SaveButton(const String& InID);

    struct ImplBase
    { 
        virtual ~ImplBase() = default;
        ImplBase(const ID& InID) : id(InID) {}
        
        virtual Base& GetData() = 0;
        
        bool Load()
        {
            if (loaded)
                return true;
            auto& data = GetData();
            data.Unload();
            data.id = id;
            loaded = data.Load();
            if (loaded)
                time = data.GetEditTime();
            return loaded;
        }
        
        bool Unload()
        {
            if (!loaded)
                return true;
            auto& data = GetData();
            loaded = !data.Unload();
            return !loaded;  
        }
        
        bool TryHotReload()
        {
            auto& data = GetData();
            CHECK_RETURN(data.GetEditTime() == time, false);
            LOG("Hot reloading resource: " + id.Str());
            Unload();
            return Load();
        }

        bool Edit(const String& InName, uint32 InOffset = 0)
        {
            auto& data = GetData();
            if (data.Edit(InName, InOffset))
            {
                if (!id.Unique())
                    data.Save();
                TryHotReload();
                return true;
            }   
            return false;
        }
        
        ID id = {};
        Utility::Timepoint time = {}; 
        uint32 count = 0;
        bool loaded = false;
    };

    template <class T>
    struct Impl final : ImplBase
    {
        Impl(const ID& InID) : ImplBase(InID) { }
        Base& GetData() override { return *Utility::Cast<Base, T>(&data); }
        T data = {};
    };
}
