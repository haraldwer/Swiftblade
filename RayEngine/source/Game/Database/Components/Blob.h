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
        void OnReadFailed(const Nakama::NError& InError);
        void OnReadSuccess(const Nakama::NStorageObjects& InVector);

    private:
        BlobData Data;
    };    
}
