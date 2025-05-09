#pragma once

#include <physx/PxShape.h>
#include <physx/PxSimulationEventCallback.h>

#include "Contact.h"
#include "PhysXUtility.h"

namespace Physics
{
    // Note:
    // Careful when including this!
    // Contains heavy includes

    class ErrorCallback : public physx::PxErrorCallback
    {
    public:
        ErrorCallback() = default;
        ~ErrorCallback() override = default;

        virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) PX_OVERRIDE
        {
            LOG("[" + Utility::ToStr(code) + "] " + message + " (" + file + ":" + Utility::ToStr(line) + ")");
        }
    };
    
    
    class Callback : public physx::PxSimulationEventCallback
    {
    public:

        static physx::PxFilterFlags contactReportFilterShader(
            physx::PxFilterObjectAttributes attributes0, const physx::PxFilterData filterData0,
            physx::PxFilterObjectAttributes attributes1, const physx::PxFilterData filterData1,
            physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
        {
            PROFILE();
            
            PX_UNUSED(constantBlockSize);
            PX_UNUSED(constantBlock);
            
            if (true)//((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)))
            {
                // Any triggers
                if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
                {
                    pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT
                        | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                        | physx::PxPairFlag::eNOTIFY_TOUCH_LOST
                        | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
                    return physx::PxFilterFlag::eDEFAULT;
                }

                // Both kinematic
                if (physx::PxFilterObjectIsKinematic(attributes0) && physx::PxFilterObjectIsKinematic(attributes1))
                    return physx::PxFilterFlag::eKILL;
                
                // Normal
                pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT
                    | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                    | physx::PxPairFlag::eNOTIFY_TOUCH_LOST
                    | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
                return physx::PxFilterFlag::eDEFAULT;
            }

            return physx::PxFilterFlag::eSUPPRESS;
        }
        
        void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override
        {
            PROFILE();
            
            PX_UNUSED((pairHeader));
            for (physx::PxU32 i = 0; i < nbPairs; i++)
            {
                const bool found = pairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
                const bool lost = pairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
                if (!found && !lost)
                    continue; 

                // Create contact struct
                Contact c;
                const void* userData0 = pairs[i].shapes[0]->userData;
                const void* userData1 = pairs[i].shapes[1]->userData;
                c.Target = ECS::PtrToEntity(userData0);
                c.Self = ECS::PtrToEntity(userData1);

                // Get contact points 
                if (const physx::PxU32 contactCount = pairs[i].contactCount)
                {
                    static Vector<physx::PxContactPairPoint> contactPoints;
                    contactPoints.resize(contactCount);
                    pairs[i].extractContacts(contactPoints.data(), contactCount);
                    for (physx::PxU32 j = 0; j < contactCount; j++)
                    {
                        auto& p = c.Points.emplace_back();
                        p.Position = Utility::PhysX::ConvertVec(contactPoints[j].position);
                        p.Impulse = Utility::PhysX::ConvertVec(contactPoints[j].impulse);
                        p.Normal = Utility::PhysX::ConvertVec(contactPoints[j].normal);
                        p.Separation = contactPoints[j].separation;
                    }
                }
                
                // Send contact to gameplay!
                if (found) RegisterContact(c, Contact::Event::BEGIN);
                if (lost) RegisterContact(c, Contact::Event::END);
            }
        }
        
        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override
        {
            PROFILE();
            
            for (physx::PxU32 i = 0; i < count; i++)
            {
                const bool found = pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
                const bool lost = pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
                if (!found && !lost)
                    continue; 
                
                // Create contact struct
                Contact c;
                const physx::PxShape* triggeredShape = pairs[i].triggerShape;
                const physx::PxShape* triggerActivator = pairs[i].otherShape;
                c.Self = ECS::PtrToEntity(triggerActivator->userData);
                c.Target = ECS::PtrToEntity(triggeredShape->userData);
                c.IsTrigger = true;

                // Send contact to gameplay!
                if (found) RegisterContact(c, Contact::Event::BEGIN);
                if (lost) RegisterContact(c, Contact::Event::END);
            }
        }

        // Unused
        void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override { PX_UNUSED(constraints); PX_UNUSED(count); }
        void onWake(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
        void onSleep(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
        void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override { }

    private:
        
        static void RegisterContact(const Contact& InContact, Contact::Event InEvent)
        {
            PROFILE();
            
            const auto func = [](const Contact& InContact, Contact::Event InEvent)
            {
                // Invoke callback on all systems
                for (auto& sys : ECS::Manager::Get().GetAllSystems())
                {
                    CHECK_ASSERT(!sys.second, "System nullptr");
                    CHECK_CONTINUE(!sys.second->Contains(InContact.Self))
                    switch (InEvent)
                    {
                    case Contact::Event::BEGIN:
                        sys.second->OnBeginContact(InContact);
                        break;
                    case Contact::Event::END:
                        sys.second->OnEndContact(InContact);
                        break;
                    }
                }
            };

            func(InContact, InEvent);
            
            // Also test flipped contact
            const Contact flippedContact = {
                InContact.Self,
                InContact.Target,
                InContact.Points,
                InContact.IsTrigger
            };
            func(flippedContact, InEvent);
        }
    };
}
