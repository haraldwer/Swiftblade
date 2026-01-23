#pragma once

namespace Audio
{
    struct Stream
    {
        
    };
    
    void InitDevice();
    void CloseDevice();
    bool IsDeviceReady();
    
    void UnloadStream(const Stream& InStream);
    Stream LoadStream(uint32 InSampleRate, uint32 InSampleSize, uint32 InChannels); 
    void SetSteamBufferSize(uint32 InSize); 
    void SetStreamCallback(const Stream& InStream, std::function<void(void*, uint32)> InCallback);
    void PlayStream(const Stream& InStream);
}