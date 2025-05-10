#pragma once

#include "imgui.h"
#include "rlImGui.h"

namespace ImGui
{
	namespace ThemeColor
	{
		struct ImColorConfig
		{
			ImVec4 base = ImVec4(152/255.0f, 183/255.0f, 215/255.0f, 1);
			ImVec4 bg = ImVec4(51/255.0f, 54/255.0f, 62/255.0f, 1);
			ImVec4 text = ImVec4(1, 1, 1, 1);
			float high_val = 0.485f;
			float mid_val = 0.640f;
			float low_val = 0.320f;
			float window_offset = -0.145f;
		};

		inline ImVec4 make_high(ImColorConfig InConfig, float alpha) {
		    ImVec4 res(0, 0, 0, alpha);
		    ColorConvertRGBtoHSV(InConfig.base.x, InConfig.base.y, InConfig.base.z, res.x, res.y, res.z);
		    res.z = InConfig.high_val;
		    ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
		    return res;
		}

		inline ImVec4 make_mid(ImColorConfig InConfig, float alpha) {
		    ImVec4 res(0, 0, 0, alpha);
		    ColorConvertRGBtoHSV(InConfig.base.x, InConfig.base.y, InConfig.base.z, res.x, res.y, res.z);
		    res.z = InConfig.mid_val;
		    ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
		    return res;
		}

		inline ImVec4 make_low(ImColorConfig InConfig, float alpha) {
		    ImVec4 res(0, 0, 0, alpha);
		    ColorConvertRGBtoHSV(InConfig.base.x, InConfig.base.y, InConfig.base.z, res.x, res.y, res.z);
		    res.z = InConfig.low_val;
		    ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
		    return res;
		}

		inline ImVec4 make_bg(ImColorConfig InConfig, float alpha, float offset = 0.f) {
		    ImVec4 res(0, 0, 0, alpha);
		    ColorConvertRGBtoHSV(InConfig.bg.x, InConfig.bg.y, InConfig.bg.z, res.x, res.y, res.z);
		    res.z += offset;
		    ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
		    return res;
		}

		inline ImVec4 make_text(ImColorConfig InConfig, float alpha) {
		    return ImVec4(InConfig.text.x, InConfig.text.y, InConfig.text.z, alpha);
		}

		inline void Apply(ImColorConfig InConfig)
		{
			ImGuiStyle& style = GetStyle();

			style.Colors[ImGuiCol_Text] = make_text(InConfig, 0.78f);
			style.Colors[ImGuiCol_TextDisabled] = make_text(InConfig, 0.28f);
			style.Colors[ImGuiCol_WindowBg] = make_bg(InConfig, 1.00f, InConfig.window_offset);
			style.Colors[ImGuiCol_ChildBg] = make_bg(InConfig, 0.58f);
			style.Colors[ImGuiCol_PopupBg] = make_bg(InConfig, 0.9f);
			style.Colors[ImGuiCol_Border] = make_bg(InConfig, 0.6f, -0.05f);
			style.Colors[ImGuiCol_BorderShadow] = make_bg(InConfig, 0.0f, 0.0f);
			style.Colors[ImGuiCol_FrameBg] = make_bg(InConfig, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = make_mid(InConfig, 0.78f);
			style.Colors[ImGuiCol_FrameBgActive] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = make_low(InConfig, 1.00f);
			style.Colors[ImGuiCol_TitleBgActive] = make_high(InConfig, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = make_bg(InConfig, 0.75f);
			style.Colors[ImGuiCol_MenuBarBg] = make_bg(InConfig, 0.47f);
			style.Colors[ImGuiCol_ScrollbarBg] = make_bg(InConfig, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = make_low(InConfig, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = make_mid(InConfig, 0.78f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = make_high(InConfig, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = make_bg(InConfig, 1.0f, .1f);
			style.Colors[ImGuiCol_SliderGrabActive] = make_high(InConfig, 1.0f);
			style.Colors[ImGuiCol_Button] = make_bg(InConfig, 1.0f, .2f);
			style.Colors[ImGuiCol_ButtonHovered] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = make_high(InConfig, 1.00f);
			style.Colors[ImGuiCol_Header] = make_mid(InConfig, 0.76f);
			style.Colors[ImGuiCol_HeaderHovered] = make_mid(InConfig, 0.86f);
			style.Colors[ImGuiCol_HeaderActive] = make_high(InConfig, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
			style.Colors[ImGuiCol_ResizeGripHovered] = make_mid(InConfig, 0.78f);
			style.Colors[ImGuiCol_ResizeGripActive] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = make_text(InConfig, 0.63f);
			style.Colors[ImGuiCol_PlotLinesHovered] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = make_text(InConfig, 0.63f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = make_mid(InConfig, 0.43f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = make_bg(InConfig, 0.73f);
			style.Colors[ImGuiCol_Tab] = make_bg(InConfig, 0.40f);
			style.Colors[ImGuiCol_TabHovered] = make_high(InConfig, 1.00f);
			style.Colors[ImGuiCol_TabActive] = make_mid(InConfig, 1.00f);
			style.Colors[ImGuiCol_TabUnfocused] = make_bg(InConfig, 0.40f);
			style.Colors[ImGuiCol_TabUnfocusedActive] = make_bg(InConfig, 0.70f);

			style.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.22f);
			style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.05f, 0.05f, 0.07f, 1.00f);
			style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.05f, 0.05f, 0.07f, 1.00f);
			style.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
			style.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.50f, 0.51f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_NavHighlight]           = ImVec4(0.27f, 0.29f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.50f, 0.51f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.18f, 0.55f, 0.50f);
		}

		inline void Select(ImColorConfig& InConfig)
		{
		    ColorEdit3("Base", (float*)&InConfig.base, ImGuiColorEditFlags_PickerHueWheel);
		    ColorEdit3("Background", (float*)&InConfig.bg, ImGuiColorEditFlags_PickerHueWheel);
		    ColorEdit3("Text", (float*)&InConfig.text, ImGuiColorEditFlags_PickerHueWheel);
		    SliderFloat("High", &InConfig.high_val, 0, 1);
		    SliderFloat("Mid", &InConfig.mid_val, 0, 1);
		    SliderFloat("Low", &InConfig.low_val, 0, 1);
		    SliderFloat("Window", &InConfig.window_offset, -0.4f, 0.4f);
			Apply(InConfig);
		}
	}
	
	inline void ThemeMoonlight()
	{
		// Fork of Moonlight style from ImThemes
		ImGuiStyle& style = GetStyle();
		
		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.3f;
		style.WindowPadding = ImVec2(12.0f, 12.0f);
		style.WindowRounding = 11.5f;
		style.WindowBorderSize = 0.0f;
		style.WindowMinSize = ImVec2(20.0f, 20.0f);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(8.0f, 3.400000095367432f);
		style.FrameRounding = 11.89999961853027f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(4.300000190734863f, 5.5f);
		style.ItemInnerSpacing = ImVec2(7.099999904632568f, 1.799999952316284f);
		style.CellPadding = ImVec2(10.0f, 5.0f);
		style.IndentSpacing = 0.0f;
		style.ColumnsMinSpacing = 4.900000095367432f;
		style.ScrollbarSize = 11.60000038146973f;
		style.ScrollbarRounding = 15.89999961853027f;
		style.GrabMinSize = 3.700000047683716f;
		style.GrabRounding = 20.0f;
		style.TabRounding = 8.0f;
		style.TabBorderSize = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		ThemeColor::ImColorConfig config;
		Apply(config);
	}

	inline static ImFont* font = nullptr;
	
	inline void LoadFont()
	{
		const ImGuiIO& io = GetIO();
		font = io.Fonts->AddFontFromFileTTF("Defaults/F_SourceCodePro.ttf", 16);
		rlImGuiReloadFonts();
	}

	inline void PushDefaultFont()
	{ 
		if (font && font->IsLoaded())
			PushFont(font);
	}

	inline void PopDefaultFont()
	{
		if (font)
			PopFont();
	}

	
}
