#pragma once
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
    static ImVec2 CalculateSectionBoundsX(float padding) {
        auto *window = GetCurrentWindow();
        float windowStart = GetWindowPos().x;

        return {windowStart + window->WindowPadding.x + padding,
            windowStart + GetWindowWidth() - window->WindowPadding.x - padding};
    }

    inline bool BeginSection(const char *title) {
        GetWindowDrawList()->ChannelsSplit(2);

        // Draw content above the rectangle
        GetWindowDrawList()->ChannelsSetCurrent(1);

        auto padding = GetStyle().WindowPadding;

        auto *window = GetCurrentWindow();
        float windowWidth = GetWindowWidth();

        auto boundsX = CalculateSectionBoundsX(padding.x);

        // Title will be clipped till the middle
        // because I am going to have a collapsing
        // header there
        const float midPoint = boundsX.x + (boundsX.y - boundsX.x) / 2.0f;

        // Start from padding position
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

        return true;
    }

    inline void EndSection() {
        auto padding = GetStyle().WindowPadding;

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

        // Since rectangle is bigger than the box, move the cursor;
        // so, it starts outside the box
        SetCursorPosY(GetCursorPosY() + padding.y);

        // Then, add default spacing
        Spacing();
    }
}
