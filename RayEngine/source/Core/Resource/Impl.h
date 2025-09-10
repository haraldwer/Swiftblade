#pragma once

#include "Identifier.h"

namespace Resource
{
    struct Base
    { 
        virtual ~Base() = default;
        Base(const ID& InID) : id(InID) {}
        virtual bool Load() = 0;
        virtual bool Unload() = 0;
        virtual bool Edit(const String& InName, uint32 InOffset = 0) = 0;
        virtual bool IsEditable() const = 0;
        virtual bool TryHotReload() = 0;

        // Editing
        static String Pick(const String& InLabel, const String& InID);
        static bool SaveButton(const String& InID);

        ID id = {};
        Utility::Timepoint editTimestamp = {}; 
        uint32 count = 0;
        bool loaded = false;
    };

    template <class T, bool Editable = false>
    struct Impl : Base
    {
        Impl(const ID& InID) : Base(InID) {}
        
        bool Load() override
        {
            if (loaded)
                return true;
            data.Unload();
            loaded = data.Load(id.Str());
            if (loaded)
                editTimestamp = data.GetEditTime();
                // TODO: Remove this requirement, maybe use template parameter? 
            return loaded;
        }
        
        bool Unload() override
        {
            if (!loaded)
                return true; 
            loaded = !data.Unload();
            return !loaded;  
        }
        
        bool TryHotReload() override
        {
            CHECK_RETURN(data.GetEditTime() == editTimestamp, false);
            LOG("Hot reloading resource: " + id.Str());
            Unload();
            return Load();
        }

        bool Edit(const String& InName, uint32 InOffset = 0) override
        {
            if (Editable)
            {
                if (data.Edit(InName, InOffset))
                {
                    if (!id.Unique())
                        data.Save(id.Str());
                    TryHotReload();
                    return true;
                }
            }   
            return false;
        }

        bool IsEditable() const override { return Editable; }

        T data = {};
    };
}
