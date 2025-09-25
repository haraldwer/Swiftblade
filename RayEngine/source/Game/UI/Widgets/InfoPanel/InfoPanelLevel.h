#pragma once
#include "Database/Data/RPCLevelInfo.h"
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
        
        LevelEntryData data;
        DB::Event<DB::RPCLevelInfo>::ContextCallback<InfoPanelLevel*> onInfo;
    };
}
