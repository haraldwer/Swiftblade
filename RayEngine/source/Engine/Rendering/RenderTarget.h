﻿#pragma once

#include "raylib.h"

class ShaderResource;

class RenderTarget
{
    struct Buffer
    {
        Texture2D Tex = {};
        String Name; 
    };
    
public:
    bool TryBeginSetup(const RenderTexture& InRenderTexture);
    void CreateBuffer(const String& InName, PixelFormat InPixelFormat);
    void EndSetup(const RenderTexture& InRenderTexture) const;
    
    void Unload();
    void BeginWrite() const;
    void EndWrite() const;

    struct Slot
    {
        int Loc = 0;
        int Index = 0;
    };
    
    void Unbind(ShaderResource& InShader, Slot& InOutSlots) const;
    void Bind(ShaderResource& InShader, Slot& InOutSlots) const;
    
    const Vector<Buffer>& GetBuffers() const { return Buffers; }
    Vec2F Size() const { return { static_cast<float>(Width), static_cast<float>(Height) }; }
    
private: 
    
    uint32 FrameBuffer = 0;
    Vector<Buffer> Buffers;

    int Width = 0;
    int Height = 0;
};
