#pragma once
#include "Database/Events.h"
#include "Database/Data/Response.h"
#include "Database/Data/RPCLevel.h"
#include "UI/Elements/Container.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"

namespace UI
{
    class InfoPanelLevel : public Container
    {
        CLASS_INFO(InfoPanelLevel, Container);
    public:
        InfoPanelLevel() : onInfo(this) {}
        InfoPanelLevel(const InfoPanelLevel& InOther) : Container(InOther), data(InOther.data), onInfo(this) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetLevel(const LevelEntryData& InData);

    private:

        void RecieveInfo(const DB::Response<DB::RPCLevelInfo>& InResponse);

        void SetEntryInfo(DB::RPCLevelList::Entry InEntry);
        void SetResourceInfo(const Level & InData);
        
        LevelEntryData data;
        DB::Event<DB::RPCLevelInfo>::ContextCallback<InfoPanelLevel*> onInfo;
    };
}
