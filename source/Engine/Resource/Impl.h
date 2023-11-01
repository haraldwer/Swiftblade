#pragma once

namespace Resource
{
    struct Base
    {
        virtual ~Base() = default;
        Base(const String& InIdentifier) : Identifier(InIdentifier) {}
        virtual bool Load() = 0;
        virtual bool Unload() = 0;
        bool Loaded = false;
        uint32 Count = 0;
        String Identifier;
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
            return Loaded;
        }
        
        bool Unload() override
        {
            if (!Loaded)
                return true; 
            Loaded = !Data.Unload();
            return !Loaded;  
        }
        
        T Data;
    };
}
