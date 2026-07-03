#pragma once
#include <QObject>
#include <array>
#include <vector>
#include <atomic>
#include <cmath>
#include <memory>
#include "AudioConsumer.h"
#include "Buffer.h"
#include "Constants.h"
#include "AudioRecorder.h"
#include "AudioConsumer.h"

/**
 * CAudioGenerator: Audio generator with multiple harmonic generations
 * Generates audio streams with configurable harmonics, effects, and recording capability
 */
class CAudioGenerator : public QObject
{
    Q_OBJECT

public:
    struct SHarmonicControls 
    {
        std::atomic<double> amplitude{0.125};
        std::atomic<double> phase{0.0};
        std::atomic<bool> enabled{true};
    };

    explicit CAudioGenerator(CAudioConsumer& audioConsumer, QObject *parent = nullptr);

    void connectSignals();
    
    // Set the fundamental frequency in Hz
    void setFundamental(double frequency);

    // Set the overall output volume from 0.0 to 1.0
    void setMasterAmplitude(double amplitude);

    // Set individual harmonic amplitude from 0.0 to 1.0
    void setHarmonicAmplitude(int harmonicIndex, double amplitude);

    // Set individual harmonic phase offset in radians
    void setHarmonicPhase(int harmonicIndex, double phase);

    // Enable or disable a specific harmonic
    void setHarmonicEnabled(int harmonicIndex, bool enabled);

    // Set distortion amount from 0.0 to 1.0
    void setDistortion(double distortion);

    // Set low pass filter cutoff from 0.0 to 1.0
    void setFilter(double filter);

    // Set reverb mix amount from 0.0 to 1.0
    void setReverbMix(double reverb);
    
    // Get the audio output buffer (int16_t for QAudioSink)
    CBuffer<int16_t>* getAudioOutputBuffer();
    
    // Get the mixed buffer (double for visualization)
    CBuffer<double>* getMixedBuffer();

    // Get the buffer for a specific harmonic (0-7)
    CBuffer<double>* getHarmonicBuffer(int index);

    // Get pointer to entire harmonic buffer array
    std::array<CBuffer<double>, AudioConstants::NUM_HARMONICS>* getHarmonicBuffers();
    
    void generateAudio();

private:
    const int m_sampleRate{AudioConstants::SAMPLE_RATE};
    const int m_numHarmonics{AudioConstants::NUM_HARMONICS};
    
    double m_currentPhase{0.0};
    double m_lastSample = 0.0;

    std::atomic<double> m_fundamental{220.0};
    std::atomic<double> m_masterAmplitude{0.7};
    std::atomic<double> m_distortion{0.0};
    std::atomic<double> m_filter{1.0};
    std::atomic<double> m_reverbMix{0.0};

    size_t m_reverbIndex{0};
    std::vector<double> m_reverbBuffer;
    
    CBuffer<int16_t> m_audioOutputBuffer;  // int16_t output for QAudioSink
    CBuffer<double> m_mixedBuffer;          // double mixed signal for visualization
    CAudioConsumer* m_audioConsumer;

    std::array<CBuffer<double>, AudioConstants::NUM_HARMONICS> m_harmonicBuffers;  // Individual harmonics (0-7)
    std::array<SHarmonicControls, AudioConstants::NUM_HARMONICS> m_harmonics{};    


};

