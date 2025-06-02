#pragma once

#include "Component.h"
#include "Database/Data/BlobData.h"

namespace DB
{
    class Blob : public Component
    {
    public:
        // Read blob
        void Init(DB::Manager* InManager) override;

        BlobData Get() const { return data; }
        void Set(const BlobData& InData);
        
    private:
        
        void OnReadFailed(const Nakama::NError& InError);
        void OnReadSuccess(const Nakama::NStorageObjects& InVector);
        void OnWriteFailed(const Nakama::NError& InError);
        void OnWriteSuccess(const Nakama::NStorageObjectAcks& InRpc);
        
        BlobData data = {};
    };    
}
