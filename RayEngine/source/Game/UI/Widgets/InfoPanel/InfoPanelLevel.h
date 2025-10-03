#pragma once
#include "Database/Events.h"
#include "Database/Data/Response.h"
#include "Database/Data/RPCLevel.h"
#include "Instance/Events.h"
#include "UI/Elements/Container.h"
#include "UI/Menus/MenuLevelSubmit.h"
#include "UI/Widgets/LevelList/LevelEntryWidget.h"

class MenuLevelPlay;

namespace UI
{
    class InfoPanelLevel : public Container
    {
        CLASS_INFO(InfoPanelLevel, Container);
    public:
        InfoPanelLevel() :
            onInfo(this),
            onSubmit(this),
            onSubmitClick(this),
            onInstanceRemoved(this) {}
        InfoPanelLevel(const InfoPanelLevel& InOther) :
            Container(InOther),
            data(InOther.data),
            onInfo(InOther.onInfo, this),
            onSubmit(InOther.onSubmit, this),
            onSubmitClick(InOther.onSubmitClick, this),
            onInstanceRemoved(InOther.onInstanceRemoved, this) {}
        
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        void SetLevel(const LevelEntryData& InData);

    private:

        void RecieveInfo(const DB::Response<DB::RPCLevelInfo>& InResponse);

        void SetEntryInfo(DB::RPCLevelList::Entry InEntry);
        void SetResourceInfo(const Level & InData);
        
        void SubmitClicked(const MenuLevelSubmit::OnClickedEvent& InE);
        bool SubmitLevel() const;
        void OnSubmitResponse(const DB::Response<DB::RPCSubmitLevel>& InResp);
        void OnEndPlay();
        
        LevelEntryData data;
        DB::Event<DB::RPCLevelInfo>::ContextCallback<InfoPanelLevel*> onInfo;
        DB::Event<DB::RPCSubmitLevel>::ContextCallback<InfoPanelLevel*> onSubmit;
        InstanceEvent<MenuLevelSubmit::OnClickedEvent>::ContextCallback<InfoPanelLevel*> onSubmitClick;
        MenuLevelSubmit* submitMenu = nullptr;
        MenuLevelPlay* playMenu = nullptr;
        bool submitting = false;

        InstanceRemovedCallbackT<InfoPanelLevel*> onInstanceRemoved;
    };
}
