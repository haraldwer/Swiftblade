#pragma once

#include "Events.h"
#include "Manager.h"
#include "Resource/Identifier.h"
#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

namespace DB
{
    struct RPCNetResource : PropertyOwner<RPCNetResource>
    {
        struct Request : PropertyOwner<Request>
        {
            PROPERTY(String, ID);            
        };

        PROPERTY(String, Json);
        PROPERTY(String, Error);
        PROPERTY(bool, Success);
    };
    
    template <class T>
    struct NetRes : Resource::Base
    {
        bool Load() override { return data.Load(); }
        bool Unload() override { return data.Unload(); }
        
        T data;
        bool cached = false;
        bool refreshed = false;
        bool requested = false;
        bool checkedCache = false;
    };
    
    template <class T, bool AllowCache>
    class NetRef
    {
        // DB callback 
        class NetCallback : public Event<RPCNetResource, Resource::ID>::ContextCallback<NetRef*>
        {
        protected:
            bool ShouldInvoke(const Response<RPCNetResource>& InEventData, const NetRef*& InContext, const Resource::ID& InCallbackData) const override
            {
                CHECK_ASSERT(!InContext, false)
                return InContext->res.Identifier() == InCallbackData;
            }
        };
    
    public:

        struct EventData
        {
            NetRef ref;
            bool success = false;
            String error;
        };

        typedef GlobalEvent<EventData, Resource::ID> Event;
        template <class ContextT>
        class Callback : public Event::template ContextCallback<ContextT>
        {
        protected:
            bool ShouldInvoke(const EventData& InData, const ContextT& InContext, const Resource::ID& InCallbackData) const override
            {
                return InData.ref.GetRes().Identifier() = InCallbackData;
            }
        };
        
        NetRef() = default;
        NetRef(const String& InID) : res(GetNetID(InID)), callback(&Load, this) { }
        NetRef(const NetRef& InOther) : res(InOther.res), callback(InOther.callback) { }

        T* Get() 
        {
            NetRes<Resource::PropertyFile<T>>* resource = res.GetData();
            CHECK_RETURN(!resource, nullptr);

            if (resource->refreshed || resource->cached)
                return resource->data.data;
        
            if (AllowCache && !resource->checkedCache)
            {
                resource->checkedCache = true;
                if (TryLoadCache())
                    if (resource->cached)
                        return &resource->data.data;
            }

            if (!resource->requested)
            {
                resource->requested = true;
                Request();
            }
            return nullptr;
        }

        Resource::Ref<Resource::PropertyFile<T>> GetResource()
        {
            auto resource = res.GetData();
            CHECK_RETURN_LOG(!resource, "Failed to get resource ptr", {});
            auto newRes = Resource::Ref<Resource::PropertyFile<T>>(res.Identifier());
            newRes.GetData()->data = resource->data.data;
            return newRes;
        }
        
    private:
        
        static void Load(const Response<RPCNetResource>& InResp, const NetRef*& InC)
        {
            CHECK_ASSERT(!InC, "Invalid context");
            InC->Load(InResp);
        }

        bool TryLoadCache()
        {
            auto resource = res.GetData();
            if (!resource)
            {
                Error("Failed to get resource ptr");
                return false;
            }

            String file = Utility::File::Read(res.Identifier().Str());
            DocumentObj doc;
            doc.Parse(file.c_str());
            const DocumentObj& docObj = doc;
            const auto obj = docObj.GetObj();
            T& data = resource->data.data;
            data.Deserialize(obj); // NetRes -> PropertyFile -> PropertyOwner
            resource->cached = true;
        }
        
        void Request()
        {
            RPCNetResource::Request request;
            request.ID = res.Identifier().Str();
            Manager::Get().rpc.Request<RPCNetResource>(request);
        }

        void Load(const Response<RPCNetResource>& InResp)
        {
            if (!InResp.success)
            {
                Error(InResp.error);
                return;
            }

            if (!InResp.data.Success)
            {
                Error(InResp.data.Error);
                return;
            }

            if (InResp.data.Json.Get().empty())
            {
                Error("No data in response");
                return;
            }
            
            if (AllowCache) // Write to cache
                if (!Utility::File::Write(res.Identifier().Str(), InResp.data))
                    Error("Failed to write to cache");

            auto resource = res.GetData();
            if (!resource)
            {
                Error("Failed to get resource ptr");
                return;
            }

            DocumentObj doc;
            doc.Parse(InResp.data.Json.Get().c_str());
            const DocumentObj& docObj = doc;
            const auto obj = docObj.GetObj();
            T& data = resource->data.data;
            data.Deserialize(obj); // NetRes -> PropertyFile -> PropertyOwner
            resource->cached = true;
            resource->refresh = true;
            Event::Invoke({ *this, true, {} });
        }

        void Error(const String& InMessage)
        {
            LOG("NetResource Error: " + InMessage)
        }
        
        static Resource::ID GetNetID(const String &InStr)
        {
            return { Utility::File::GetCachePath(InStr, ".json"), false };
        }
        
        Resource::Ref<NetRes<Resource::PropertyFile<T>>> res;
        NetCallback callback;
    };
}
