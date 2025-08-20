#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    class InfoPanel : public Container
    {
        TYPE_INFO(InfoPanel, Container);
    public:
        void Init(Container& InOwner) override;
        void SetRoom(String InObj);
        void SetLevel(String InObj);
        void Populate(); 

    private:
        String object;
        ElementID root = -1;
    };
}
