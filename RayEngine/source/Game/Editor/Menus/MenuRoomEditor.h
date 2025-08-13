#pragma once
#include "Instance/Instance.h"

class MenuRoomEditor : Menu::Instance
{
public:
    void Init() override;
    void Update() override;
    bool IsBlocking() const override { return false; }

    String GetSelected() const { return selected; }
    void SetSelected(const String& InStr);

private:
    Array<String, 5> options = {
        "Connection",
        "Path",
        "Generate",
        "Block",
        "Objects",
    };
    String selected;
};
