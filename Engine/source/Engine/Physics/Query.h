#pragma once
#include "ContactHandler.h"
#include "QueryStructs.h"

namespace Physics
{
    class Query
    {
        friend class Manager;
    public:
        static QueryResult Trace(const TraceParams& InParams);
        static QueryResult Sweep(const SweepParams& InParams);
        static Vector<Contact> GetContacts(ECS::EntityID InID);
    };
}
