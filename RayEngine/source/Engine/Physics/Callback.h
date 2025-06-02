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

        void reportError(const physx::PxErrorCode::Enum InCode, const char* InMessage, const char* InFile, const int InLine) PX_OVERRIDE
        {
            LOG("[" + Utility::ToStr(InCode) + "] " + InMessage + " (" + InFile + ":" + Utility::ToStr(InLine) + ")");
        }
    };
    
    
    class Callback : public physx::PxSimulationEventCallback
    {
    public:

        static physx::PxFilterFlags contactReportFilterShader(
            const physx::PxFilterObjectAttributes InAttributes0, const physx::PxFilterData InFilterData0,
            const physx::PxFilterObjectAttributes InAttributes1, const physx::PxFilterData InFilterData1,
            physx::PxPairFlags& InPairFlags, const void* InConstantBlock, physx::PxU32 InConstantBlockSize)
        {
            PROFILE();
            
            PX_UNUSED(InConstantBlockSize);
            PX_UNUSED(InConstantBlock);
            
            if (true)//((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)))
            {
                // Any triggers
                if (physx::PxFilterObjectIsTrigger(InAttributes0) || physx::PxFilterObjectIsTrigger(InAttributes1))
                {
                    InPairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT
                        | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                        | physx::PxPairFlag::eNOTIFY_TOUCH_LOST
                        | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
                    return physx::PxFilterFlag::eDEFAULT;
                }

                // Both kinematic
                if (physx::PxFilterObjectIsKinematic(InAttributes0) && physx::PxFilterObjectIsKinematic(InAttributes1))
                    return physx::PxFilterFlag::eKILL;
                
                // Normal
                InPairFlags = physx::PxPairFlag::eCONTACT_DEFAULT
                    | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                    | physx::PxPairFlag::eNOTIFY_TOUCH_LOST
                    | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
                return physx::PxFilterFlag::eDEFAULT;
            }

            return physx::PxFilterFlag::eSUPPRESS;
        }
        
        void onContact(const physx::PxContactPairHeader& InPairHeader, const physx::PxContactPair* InPairs, physx::PxU32 InNbPairs) override
        {
            PROFILE();
            
            PX_UNUSED((InPairHeader));
            for (physx::PxU32 i = 0; i < InNbPairs; i++)
            {
                const bool found = InPairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
                const bool lost = InPairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
                if (!found && !lost)
                    continue; 

                // Create contact struct
                Contact c;
                const void* userData0 = InPairs[i].shapes[0]->userData;
                const void* userData1 = InPairs[i].shapes[1]->userData;
                c.target = ECS::PtrToEntity(userData0);
                c.self = ECS::PtrToEntity(userData1);

                // Get contact points 
                if (const physx::PxU32 contactCount = InPairs[i].contactCount)
                {
                    static Vector<physx::PxContactPairPoint> contactPoints;
                    contactPoints.resize(contactCount);
                    InPairs[i].extractContacts(contactPoints.data(), contactCount);
                    for (physx::PxU32 j = 0; j < contactCount; j++)
                    {
                        auto& p = c.points.emplace_back();
                        p.position = Utility::PhysX::ConvertVec(contactPoints[j].position);
                        p.impulse = Utility::PhysX::ConvertVec(contactPoints[j].impulse);
                        p.normal = Utility::PhysX::ConvertVec(contactPoints[j].normal);
                        p.separation = contactPoints[j].separation;
                    }
                }
                
                // Send contact to gameplay!
                if (found) RegisterContact(c, Contact::Event::BEGIN);
                if (lost) RegisterContact(c, Contact::Event::END);
            }
        }
        
        void onTrigger(physx::PxTriggerPair* InPairs, const physx::PxU32 InCount) override
        {
            PROFILE();
            
            for (physx::PxU32 i = 0; i < InCount; i++)
            {
                const bool found = InPairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
                const bool lost = InPairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
                if (!found && !lost)
                    continue; 
                
                // Create contact struct
                Contact c;
                const physx::PxShape* triggeredShape = InPairs[i].triggerShape;
                const physx::PxShape* triggerActivator = InPairs[i].otherShape;
                c.self = ECS::PtrToEntity(triggerActivator->userData);
                c.target = ECS::PtrToEntity(triggeredShape->userData);
                c.isTrigger = true;

                // Send contact to gameplay!
                if (found) RegisterContact(c, Contact::Event::BEGIN);
                if (lost) RegisterContact(c, Contact::Event::END);
            }
        }

        // Unused
        void onConstraintBreak(physx::PxConstraintInfo* InConstraints, const physx::PxU32 InCount) override { PX_UNUSED(InConstraints); PX_UNUSED(InCount); }
        void onWake(physx::PxActor** InActors, const physx::PxU32 InCount) override { PX_UNUSED(InActors); PX_UNUSED(InCount); }
        void onSleep(physx::PxActor** InActors, const physx::PxU32 InCount) override { PX_UNUSED(InActors); PX_UNUSED(InCount); }
        void onAdvance(const physx::PxRigidBody* const* InBodyBuffer, const physx::PxTransform* InPoseBuffer, const physx::PxU32 InCount) override { }

    private:
        
        static void RegisterContact(const Contact& InContact, const Contact::Event InEvent)
        {
            PROFILE();
            
            const auto func = [](const Contact& InCont, const Contact::Event InEv)
            {
                // Invoke callback on all systems
                for (auto& sys : ECS::Manager::Get().GetAllSystems())
                {
                    CHECK_ASSERT(!sys.second, "System nullptr");
                    CHECK_CONTINUE(!sys.second->Contains(InCont.self))
                    switch (InEv)
                    {
                    case Contact::Event::BEGIN:
                        sys.second->OnBeginContact(InCont);
                        break;
                    case Contact::Event::END:
                        sys.second->OnEndContact(InCont);
                        break;
                    }
                }
            };

            func(InContact, InEvent);
            
            // Also test flipped contact
            const Contact flippedContact = {
                InContact.self,
                InContact.target,
                InContact.points,
                InContact.isTrigger
            };
            func(flippedContact, InEvent);
        }
    };
}
