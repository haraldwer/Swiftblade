#pragma once

namespace Resource
{
    struct Base
    {
        virtual ~Base() = default;
        virtual bool Load(const String& InIdentifier) = 0;
        virtual bool Unload() = 0;
        bool Loaded = false;
        uint32 Count = 0;
        String Identifier;
    };

    template <class T>
    struct Impl : Base
    {
        bool Load(const String& InIdentifier) override
        {
            Identifier = InIdentifier; 
            if (Loaded)
                return true; 
            Loaded = Data.Load(InIdentifier);
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
