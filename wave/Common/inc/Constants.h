#pragma once
#include <cstring>

namespace AudioConstants
{
    constexpr int    SAMPLE_RATE      = 44100;
    constexpr int    MAX_HARMONICS    = 8;
    constexpr int    NUM_HARMONICS    = MAX_HARMONICS;
    constexpr size_t BUFFER_SIZE      = 8192;
    constexpr size_t DISPLAY_SAMPLES = 2048;
    constexpr size_t FFT_SIZE      = 2048;
    
    // Samples to read per GUI frame: BUFFER_SIZE / 60fps ≈ 136 samples
    // This allows smooth display of consumed audio buffer over 60 frames
    constexpr size_t SAMPLES_PER_FRAME = BUFFER_SIZE / 60;
}