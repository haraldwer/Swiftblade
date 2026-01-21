#include "Audio.h"

// TODO!

void Audio::InitDevice() {}
void Audio::CloseDevice() {}
bool Audio::IsDeviceReady() { return false; }
void Audio::UnloadStream(const Stream &InStream) {}
Audio::Stream Audio::LoadStream(uint32 InSampleRate, uint32 InSampleSize, uint32 InChannels) { return {}; }
void Audio::SetSteamBufferSize(uint32 InSize) {}
void Audio::SetStreamCallback(const Stream &InStream, std::function<void(void *, uint32)> InCallback) {}
void Audio::PlayStream(const Stream &InStream) {}
