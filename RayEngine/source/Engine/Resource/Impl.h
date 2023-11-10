#pragma once
#include "Utility/Time/Timepoint.h"

namespace Resource
{
    class Base
    {
    public: 
        virtual ~Base() = default;
        Base(const String& InIdentifier) : Identifier(InIdentifier) {}
        virtual bool Load() = 0;
        virtual bool Unload() = 0;
        virtual bool TryHotReload() = 0;

    protected:
        bool Loaded = false;
        uint32 Count = 0;
        String Identifier;
        Utility::Timepoint EditTimestamp; 

        static String Pick(const String& InLabel, const String& InID);
    };

    template <class T>
    struct Impl : Base
    {
        Impl(const String& InIdentifier) : Base(InIdentifier) {}
        
        bool Load() override
        {
            if (Loaded)
                return true; 
            Loaded = Data.Load(Identifier);
            if (Loaded)
                EditTimestamp = Data.GetEditTime();
                // TODO: Remove this requirement, maybe use template parameter? 
            return Loaded;
        }
        
        bool Unload() override
        {
            if (!Loaded)
                return true; 
            Loaded = !Data.Unload();
            return !Loaded;  
        }
        
        bool TryHotReload() override
        {
            CHECK_RETURN(Data.GetEditTime() == EditTimestamp, false);
            LOG("Hot reloading resource: " + Identifier);
            Unload();
            return Load();
        }
        
        T Data;
    };
}
