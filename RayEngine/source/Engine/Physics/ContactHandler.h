#pragma once
#include "Contact.h"
#include "pch.h"
#include "reactphysics3d/engine/EventListener.h"
#include "Utility.h"

namespace Physics
{
    class ContactHandler : public reactphysics3d::EventListener
    {
    public:
        
        void Clear()
        {
            contacts.clear();
        }

        Vector<Contact> GetContacts(const ECS::EntityID& InID)
        {
            auto find = contacts.find(InID);
            if (find != contacts.end())
                return find->second;
            return {};
        }
        
    private:
        void onContact(const CallbackData& callbackData) override
        {
            for (uint32 p = 0; p < callbackData.getNbContactPairs(); p++)
            {
                ContactPair contactPair = callbackData.getContactPair(p);
                Contact result;
                
                switch (contactPair.getEventType())
                {
                    case ContactPair::EventType::ContactStart:
                        result.type = Contact::Event::BEGIN;
                        break;
                    case ContactPair::EventType::ContactStay:
                        result.type = Contact::Event::STAY;
                        break;
                    case ContactPair::EventType::ContactExit:
                        result.type = Contact::Event::END;
                        break;
                }
                
                void* bodyData1 = contactPair.getBody1()->getUserData();
                void* bodyData2 = contactPair.getBody2()->getUserData();
                result.first = reinterpret_cast<ECS::EntityID>(bodyData1);
                result.second = reinterpret_cast<ECS::EntityID>(bodyData2);

                // Maybe use collider data instead of rb data?
                //void* colliderData1 = contactPair.getCollider1()->getUserData();
                //void* colliderData2 = contactPair.getCollider2()->getUserData();
                //ECS::EntityID colliderID1 = reinterpret_cast<ECS::EntityID>(bodyData1);
                //ECS::EntityID colliderID2 = reinterpret_cast<ECS::EntityID>(bodyData2);
                
                for (uint32 c = 0; c < contactPair.getNbContactPoints(); c++)
                {
                    ContactPoint contactPoint = contactPair.getContactPoint(c);
                    reactphysics3d::Vector3 worldPoint1 = contactPair.getCollider1()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();
                    reactphysics3d::Vector3 worldPoint2 = contactPair.getCollider2()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider2();
                    auto& point = result.points.emplace_back();
                    point.position = (GetVec(worldPoint1) + GetVec(worldPoint2)) * 0.5f;
                    point.firstPoint = GetVec(worldPoint1);
                    point.secondPoint = GetVec(worldPoint2);
                    point.normal = GetVec(contactPoint.getWorldNormal());
                    point.separation = contactPoint.getPenetrationDepth();
                }
                
                contacts[result.first].push_back(result);
                contacts[result.second].push_back(result);
            }
        }

        void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData) override
        {
            for (uint32 p = 0; p < callbackData.getNbOverlappingPairs(); p++)
            {
                reactphysics3d::OverlapCallback::OverlapPair pair = callbackData.getOverlappingPair(p);
                Contact result;
                result.isTrigger = true;

                switch (pair.getEventType())
                {
                    case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart:
                        result.type = Contact::Event::BEGIN;
                        break;
                    case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay:
                        result.type = Contact::Event::STAY;
                        break;
                    case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit:
                        result.type = Contact::Event::END;
                        break;
                }
                
                void* bodyData1 = pair.getBody1()->getUserData();
                void* bodyData2 = pair.getBody2()->getUserData();
                result.first = reinterpret_cast<ECS::EntityID>(bodyData1);
                result.second = reinterpret_cast<ECS::EntityID>(bodyData2);

                contacts[result.first].push_back(result);
                contacts[result.second].push_back(result);
            }
        }

        Map<ECS::EntityID, Vector<Contact>> contacts;
    };

    
}
