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
	
	inline void Theme2()
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

	inline void Theme1()
	{
		// AdobeInspired style by nexacopic from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();
		
		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6000000238418579f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 4.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 4.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 4.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(4.0f, 3.0f);
		style.FrameRounding = 4.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 4.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 20.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
		style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.1630901098251343f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.08627451211214066f, 0.08627451211214066f, 0.08627451211214066f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354080677f, 0.105882354080677f, 0.105882354080677f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8784313797950745f, 0.8784313797950745f, 0.8784313797950745f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9803921580314636f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.1490196138620377f, 0.1490196138620377f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2470588237047195f, 0.2470588237047195f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3294117748737335f, 0.3294117748737335f, 0.3294117748737335f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.3098039329051971f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 0.7803921699523926f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 0.6705882549285889f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.9490196108818054f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.2235294133424759f, 0.2235294133424759f, 0.2235294133424759f, 0.8627451062202454f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.321568638086319f, 0.800000011920929f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 0.9725490212440491f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.4235294163227081f, 0.4235294163227081f, 0.4235294163227081f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
	}

	inline static ImFont* font = nullptr;
	
	inline void LoadFont(float InScale)
	{
		const ImGuiIO& io = GetIO();
		font = io.Fonts->AddFontFromFileTTF("Defaults/F_FiraCode.ttf", 16 * InScale);
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
