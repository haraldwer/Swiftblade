#pragma once

namespace Resource
{
    struct Base
    { 
        virtual ~Base() = default;
        Base(String InIdentifier) : identifier(std::move(InIdentifier)) {}
        virtual bool Load() = 0;
        virtual bool Unload() = 0;
        virtual bool TryHotReload() = 0;

        // Editing
        static String Pick(const String& InLabel, const String& InID);
        static bool SaveButton(const String& InID);

        String identifier = {};
        Utility::Timepoint editTimestamp = {}; 
        uint32 count = 0;
        bool loaded = false;
    };

    template <class T>
    struct Impl : Base
    {
        Impl(const String& InIdentifier) : Base(InIdentifier) {}
        
        bool Load() override
        {
            if (loaded)
                return true;
            data.Unload();
            loaded = data.Load(identifier);
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
            LOG("Hot reloading resource: " + identifier);
            Unload();
            return Load();
        }
        
        T data = {};
    };
}
