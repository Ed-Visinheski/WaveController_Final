#pragma once
#include <cstring>

namespace AudioConstants
{
    constexpr int   SAMPLE_RATE      = 44100;
    constexpr int   MAX_HARMONICS    = 8;
    constexpr int   NUM_HARMONICS    = MAX_HARMONICS;
    constexpr size_t BUFFER_SIZE = 8192;
}