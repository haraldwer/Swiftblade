#pragma once

class DebugUI
{
public:
    DebugUI();
    virtual ~DebugUI();

    virtual void DrawDebugUI() = 0;

    bool IsDebugUIOpen() const { return Open; };
    void ToggleDebugUI() { Open = !Open; }

private:
    bool Open = true;
};
