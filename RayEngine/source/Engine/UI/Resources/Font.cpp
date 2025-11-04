#include "Font.h"

#include "Utility/File/File.h"
#include "raylib.h"
#include "utils.h"
#include "external/stb_rect_pack.h"
#include "ImGui/rlImGui.h"

bool UI::FontResource::Load()
{
    // Maybe preload a certain size? 
    return true;
}

bool UI::FontResource::Unload()
{
    for (auto& ptr : sizes)
    {
        if (ptr.second)
        {
            UnloadFont(*ptr.second);
            delete (ptr.second);
        }
    }
    sizes.clear();
    return true;
}

// Modified version of GenImageFontAtlas() for only loading glyphs
// The image is loaded from disk instead.
void GenFontGlyphs(const GlyphInfo *glyphs, Rectangle **glyphRecs, int glyphCount, int fontSize, int padding, int packMethod)
{
    if (glyphs == nullptr)
    {
        TRACELOG(LOG_WARNING, "FONT: Provided chars info not valid");
        return;
    }
    
    *glyphRecs = nullptr;

    // In case no chars count provided we suppose default of 95
    glyphCount = (glyphCount > 0)? glyphCount : 95;

    // NOTE: Rectangles memory is loaded here!
    const auto recs = static_cast<Rectangle*>(RL_MALLOC(glyphCount*sizeof(Rectangle)));

    // Calculate image size based on total glyph width and glyph row count
    int totalWidth = 0;
    int maxGlyphWidth = 0;

    for (int i = 0; i < glyphCount; i++)
    {
        if (glyphs[i].image.width > maxGlyphWidth) maxGlyphWidth = glyphs[i].image.width;
        totalWidth += glyphs[i].image.width + 2*padding;
    }

    int width = 0;
    int height = 0;
    
    //#define SUPPORT_FONT_ATLAS_SIZE_CONSERVATIVE
#if defined(SUPPORT_FONT_ATLAS_SIZE_CONSERVATIVE)
    int rowCount = 0;
    int imageSize = 64;  // Define minimum starting value to avoid unnecessary calculation steps for very small images

    // NOTE: maxGlyphWidth is maximum possible space left at the end of row
    while (totalWidth > (imageSize - maxGlyphWidth)*rowCount)
    {
        imageSize *= 2;                                 // Double the size of image (to keep POT)
        rowCount = imageSize/(fontSize + 2*padding);    // Calculate new row count for the new image size
    }

    width = imageSize;   // Atlas bitmap width
    height = imageSize;  // Atlas bitmap height
#else
    int paddedFontSize = fontSize + 2*padding;
    // No need for a so-conservative atlas generation
    float totalArea = totalWidth*paddedFontSize*1.2f;
    float imageMinSize = sqrtf(totalArea);
    int imageSize = (int)powf(2, ceilf(logf(imageMinSize)/logf(2)));

    if (totalArea < ((imageSize*imageSize)/2))
    {
        width = imageSize;    // Atlas bitmap width
        height = imageSize/2; // Atlas bitmap height
    }
    else
    {
        width = imageSize;   // Atlas bitmap width
        height = imageSize;  // Atlas bitmap height
    }
#endif

    // DEBUG: We can see padding in the generated image setting a gray background...
    //for (int i = 0; i < atlas.width*atlas.height; i++) ((unsigned char *)atlas.data)[i] = 100;

    if (packMethod == 0)   // Use basic packing algorithm
    {
        int offsetX = padding;
        int offsetY = padding;

        // NOTE: Using simple packaging, one char after another
        for (int i = 0; i < glyphCount; i++)
        {
            // Check remaining space for glyph
            if (offsetX >= (width - glyphs[i].image.width - 2*padding))
            {
                offsetX = padding;

                // NOTE: Be careful on offsetY for SDF fonts, by default SDF
                // use an internal padding of 4 pixels, it means char rectangle
                // height is bigger than fontSize, it could be up to (fontSize + 8)
                offsetY += (fontSize + 2*padding);

                if (offsetY > (height - fontSize - padding))
                {
                    for (int j = i + 1; j < glyphCount; j++)
                    {
                        TRACELOG(LOG_WARNING, "FONT: Failed to package character (%i)", j);
                        // Make sure remaining recs contain valid data
                        recs[j].x = 0;
                        recs[j].y = 0;
                        recs[j].width = 0;
                        recs[j].height = 0;
                    }
                    break;
                }
            }

            // Fill chars rectangles in atlas info
            recs[i].x = (float)offsetX;
            recs[i].y = (float)offsetY;
            recs[i].width = (float)glyphs[i].image.width;
            recs[i].height = (float)glyphs[i].image.height;

            // Move atlas position X for next character drawing
            offsetX += (glyphs[i].image.width + 2*padding);
        }
    }
    else if (packMethod == 1)  // Use Skyline rect packing algorithm (stb_pack_rect)
    {
        stbrp_context *context = static_cast<stbrp_context*>(RL_MALLOC(sizeof(*context)));
        stbrp_node *nodes = static_cast<stbrp_node*>(RL_MALLOC(glyphCount*sizeof(*nodes)));

        stbrp_init_target(context, width, height, nodes, glyphCount);
        stbrp_rect *rects = static_cast<stbrp_rect*>(RL_MALLOC(glyphCount*sizeof(stbrp_rect)));

        // Fill rectangles for packaging
        for (int i = 0; i < glyphCount; i++)
        {
            rects[i].id = i;
            rects[i].w = glyphs[i].image.width + 2*padding;
            rects[i].h = glyphs[i].image.height + 2*padding;
        }

        // Package rectangles into atlas
        stbrp_pack_rects(context, rects, glyphCount);

        for (int i = 0; i < glyphCount; i++)
        {
            // It returns char rectangles in atlas
            recs[i].x = rects[i].x + (float)padding;
            recs[i].y = rects[i].y + (float)padding;
            recs[i].width = (float)glyphs[i].image.width;
            recs[i].height = (float)glyphs[i].image.height;

            if (!rects[i].was_packed)
                TRACELOG(LOG_WARNING, "FONT: Failed to package character (%i)", i);
        }

        RL_FREE(rects);
        RL_FREE(nodes);
        RL_FREE(context);
    }

    *glyphRecs = recs;
}

bool UI::FontResource::LoadSDFFont(uint32 InSize, Font*& InFontEntry) const
{
    CHECK_ASSERT(InFontEntry, "Font already set");
    
    InFontEntry = new Font();

    int fileSize = 0;
    unsigned char *fileData = LoadFileData(id.Str().c_str(), &fileSize);
    CHECK_RETURN_LOG(!fileData, "Failed to load font: " + id.Str(), false);
        
    // SDF font generation from TTF font
    InFontEntry->baseSize = static_cast<int>(InSize);
    InFontEntry->glyphCount = 95;
    InFontEntry->glyphPadding = 0;
        
    // Parameters > font size: auto, no glyphs array provided (0), glyphs count: 0 (defaults to 95)
    InFontEntry->glyphs = LoadFontData(
        fileData,
        fileSize,
        InFontEntry->baseSize,
        nullptr,
        InFontEntry->glyphCount,
        FONT_SDF);
    
    String cachePath = GetCachePath(InSize);
    if (Utility::FileExists(cachePath))
    {
        GenFontGlyphs(
            InFontEntry->glyphs,
            &InFontEntry->recs,
            InFontEntry->glyphCount,
            InFontEntry->baseSize,
            InFontEntry->glyphPadding,
            1);

        InFontEntry->texture = LoadTexture(cachePath.c_str());
        InFontEntry->texture.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        SetTextureFilter(InFontEntry->texture, TEXTURE_FILTER_BILINEAR);
    }
    else
    {
        // Parameters > glyphs count: 95, font size: auto, glyphs padding in image: auto, pack method: 1 (Skyline algorythm)
        Image atlas = GenImageFontAtlas(
            InFontEntry->glyphs,
            &InFontEntry->recs,
            InFontEntry->glyphCount,
            InFontEntry->baseSize,
            InFontEntry->glyphPadding,
            1);

        InFontEntry->texture = LoadTextureFromImage(atlas);
        InFontEntry->texture.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        SetTextureFilter(InFontEntry->texture, TEXTURE_FILTER_BILINEAR);

        Utility::CreateDir(cachePath);
        if (ExportImage(atlas, cachePath.c_str()))
        {
            LOG("Font cached: " + cachePath);
        }
        else
        {
            LOG("Failed to cache font: " + cachePath);
        }

        UnloadImage(atlas);
        UnloadFileData(fileData);
    }
    
    return true;
}

bool UI::FontResource::LoadFont(uint32 InSize, Font *&InFontEntry)
{
    CHECK_ASSERT(InFontEntry, "Font already set");
    InFontEntry = new Font();
    (*InFontEntry) = LoadFontEx(id.Str().c_str(), InSize, nullptr, 0);
    return IsFontValid(*InFontEntry);
}

Font* UI::FontResource::Get(const uint32 InSize)
{
    CHECK_RETURN(InSize < minSize, nullptr);

    // Clamp size
    uint32 size = Utility::Math::Min(maxSize, InSize); 

    // Group similar sizes!
    uint32 roundSize = ((size + 5) / 10) * 10; 
    if (sizes.contains(roundSize))
        return sizes.at(roundSize);
    auto& f = sizes[roundSize];

#ifdef __EMSCRIPTEN__
    if (LoadFont(roundSize, f))
        return f;
#else
    if (LoadSDFFont(roundSize, f))
        return f;
#endif


    return f;
}

Shader* UI::FontResource::GetShader() const
{
#ifndef __EMSCRIPTEN__
    if (const auto res = sdfShader.Get())
        return res->Get();
#endif
    return nullptr;
}

#ifdef IMGUI_ENABLE

bool UI::FontResource::Edit(const String &InName, uint32 InOffset)
{
    ImGui::Text("Loaded sizes: %i", static_cast<int>(sizes.size()));
    for (auto& s : sizes)
    {
        CHECK_CONTINUE(!s.second)
        if (ImGui::CollapsingHeader(("Size: " + Utility::ToStr(s.first) + "##" + InName).c_str()))
        {
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
            const float mul = static_cast<float>(s.second->texture.width) / size.width;
            SetTextureWrap(s.second->texture, TEXTURE_WRAP_REPEAT);
            rlImGuiImageRect(
                &s.second->texture,
                static_cast<int>(size.x),
                static_cast<int>(static_cast<float>(s.second->texture.height) / mul),
                Rectangle{
                    0, 0,
                    static_cast<float>(s.second->texture.width),
                    static_cast<float>(-s.second->texture.height)
                });
        }
    }
    return false;
}

#else

bool UI::FontResource::Edit(const String &InName, uint32 InOffset) { return false; }

#endif

String UI::FontResource::GetCachePath(const int InSize) const
{
    return Utility::GetCachePath(id.Str() + Utility::ToStr(InSize), ".png");
}
