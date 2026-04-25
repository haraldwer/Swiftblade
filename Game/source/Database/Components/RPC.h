#pragma once
#include "Component.h"
#include "Database/Events.h"
#include "Database/NakamaTypes.h"

namespace DB
{
    class RPC : public Component
    {
    public:
        
        template <class T>
        void Request(const T::Request& InRequest)
        {
            const String json = InRequest.ToStr(false);
            auto success = [&](const Nakama::NRpc& InRpc)
            {
                String payload = GetPayload(InRpc);
                DocumentObj doc = {};
                doc.Parse(payload.c_str());
                const DocumentObj& docObj = doc;
                const auto obj = docObj.GetObj();
                
                Response<T> resp;
                resp.success = false;
                Utility::Deserialize(obj, String("success"), resp.success);
                if (resp.success)
                {
                    if (!obj.HasMember("payload"))
                    {
                        resp.success = false;
                        resp.error = "Failed to deserialize payload | " + payload;
                    }
                    else
                    {
                        Utility::ReadValue(obj["payload"].GetObj(), resp.data);
                    }
                }
                else
                {
                    Utility::Deserialize(obj, "payload", resp.error);
                }
                
                if (!resp.success)
                {
                    if (resp.error.empty())
                    {
                        LOG("RPC Unknown error");
                    }
                    else
                    {
                        LOG("RPC Error: " + resp.error);
                    }
                }
                else
                {
                    LOG("RPC Response: " + payload);
                }
                
                Event<T>().Invoke(resp);
            };
            
            auto error = [&](const Nakama::NError& InError)
            {
                Response<T> resp;
                resp.success = false;
                resp.error = GetError(InError);
                LOG("RPC Error: " + resp.error);
                Event<T>().Invoke(resp);
            };
            
            CallRPC(T::RPC(), json, success, error);
        }
    
    private:

        void CallRPC(
            const String &InRPC,
            const String& InData,
            const std::function<void(const Nakama::NRpc&)>& InSuccess,
            const std::function<void(const Nakama::NError&)>&InError) const;

        static String GetError(const Nakama::NError& InError);
        static String GetPayload(const Nakama::NRpc& InRpc);
    };
}