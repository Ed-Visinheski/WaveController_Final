#pragma once
#include <cstring>

namespace AudioConstants
{
    constexpr int   SAMPLE_RATE      = 44100;
    constexpr int   MAX_HARMONICS    = 16; //Currently unused maybe be helpful to allow user to decide on number of harmonics
    constexpr int   NUM_HARMONICS    = 5;
    constexpr size_t BUFFER_SIZE = 4096;
}