#pragma once
#include "Database/NakamaTypes.h"

namespace DB
{
    class Manager;
    
    class Component
    {
    public:
        virtual ~Component() = default;
        virtual void Init(Manager* InManager);
        virtual void Deinit();
        
        Manager& GetDB() const;
        Client& GetClient() const;
        RtClient& GetRealtimeClient() const;
        Session& GetSession() const;
        
    private:
        Manager* manager = nullptr;
    };
}
