#pragma once
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
    static ImVec2 CalculateSectionBoundsX(float padding)
    {
        auto *window = GetCurrentWindow();
        float windowStart = GetWindowPos().x;

        return {windowStart + window->WindowPadding.x + padding,
            windowStart + GetWindowWidth() - window->WindowPadding.x - padding};
    }

    inline bool BeginSection(bool InTopPadding)
    {
        GetWindowDrawList()->ChannelsSplit(2);

        // Draw content above the rectangle
        GetWindowDrawList()->ChannelsSetCurrent(1);

        auto padding = GetStyle().WindowPadding;
        padding.x *= 0.5f;
        padding.y *= 0.5f;
        PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

        auto *window = GetCurrentWindow();
        auto boundsX = CalculateSectionBoundsX(padding.x);
        const float midPoint = boundsX.x + (boundsX.y - boundsX.x) / 2.0f;

        // Start from padding position
        if (InTopPadding)
            SetCursorPosY(GetCursorPosY() + padding.y);
        BeginGroup();
        if (padding.x > 0) {
            Indent(padding.x);
        }

        PushClipRect(ImVec2(boundsX.x, window->ClipRect.Min.y),
            ImVec2(midPoint, window->ClipRect.Max.y), false);
        //Text("%s", title);
        PopClipRect();

        // Setting clip rectangle for the group contents;
        // so, that text does not overflow outside this widget
        // the parent window is resized
        PushClipRect(ImVec2(boundsX.x, window->ClipRect.Min.y),
            ImVec2(boundsX.y, window->ClipRect.Max.y), false);

        PopStyleVar();
        
        return true;
    }

    inline void EndSection()
    {
        auto padding = GetStyle().WindowPadding * 0.5f;

        PopClipRect();
        if (padding.x > 0) {
            Unindent(padding.x);
        }
        EndGroup();

        // Essentially, the content is drawn with padding
        // while the rectangle is drawn without padding
        auto boundsX = CalculateSectionBoundsX(0.0f);

        // GetItemRectMin is going to include the padding
        // as well; so, remove it
        auto panelMin = ImVec2(boundsX.x, GetItemRectMin().y - padding.y);
        auto panelMax = ImVec2(boundsX.y, GetItemRectMax().y + padding.y);

        // Draw rectangle below
        GetWindowDrawList()->ChannelsSetCurrent(0);
        GetWindowDrawList()->AddRectFilled(
            panelMin, panelMax,
            GetColorU32(GetStyleColorVec4(ImGuiCol_ChildBg)),
            0.0f);
        GetWindowDrawList()->ChannelsMerge();

        // Then, add default spacing
        Spacing();
    }
}
