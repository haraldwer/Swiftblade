#include "Generator.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

Audio::Generator::~Generator()
{
    if (audioThread.joinable())
        audioThread.join();
}

void Audio::Generator::Init()
{
    run = true;
    audioThread = std::thread([&]()
    {
        ThreadWork();
    });
}

void Audio::Generator::Deinit()
{
    run = false;
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    MA_ASSERT(pDevice->playback.channels == DEVICE_CHANNELS);

    ma_waveform* pSineWave = static_cast<ma_waveform*>(pDevice->pUserData);
    MA_ASSERT(pSineWave != NULL);

    ma_waveform_read_pcm_frames(pSineWave, pOutput, frameCount, nullptr);

    (void)pInput;   /* Unused. */
}


void Audio::Generator::ThreadWork()
{
    PROFILE_AU();
    
    // Create resources

    // Update

    ma_waveform sineWave;
    ma_device_config deviceConfig;
    ma_device device;
    ma_waveform_config sineWaveConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &sineWave;

    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        LOG("Failed to open playback device.\n");
        return;
    }

    LOG(("Device Name: %s\n", device.playback.name));

    sineWaveConfig = ma_waveform_config_init(device.playback.format, device.playback.channels, device.sampleRate, ma_waveform_type_sine, 0.2, 220);
    ma_waveform_init(&sineWaveConfig, &sineWave);

    if (ma_device_start(&device) != MA_SUCCESS) {
        LOG("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return;
    }
    
    // Free resources
}

void Audio::Generator::Tick()
{
    PROFILE_AU();
    
    // Working config...
    auto& conf = config.SwapBack();
    
    feedback.SwapFront();

    std::this_thread::yield();
}
