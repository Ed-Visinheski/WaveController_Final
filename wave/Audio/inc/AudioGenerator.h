#pragma once

#include <QIODevice>
#include <array>
#include <vector>
#include <atomic>
#include <cmath>
#include <memory>
#include "Buffer.h"
#include "Constants.h"
#include "AudioRecorder.h"

/**
 * CAudioGenerator: Audio generator with multiple harmonic generations
 * Generates audio streams with configurable harmonics, effects, and recording capability
 */
class CAudioGenerator : public QIODevice 
{
    Q_OBJECT

public:
    struct SHarmonicControls 
    {
        std::atomic<double> amplitude{0.125};
        std::atomic<double> phase{0.0};
        std::atomic<bool> enabled{true};
    };

    explicit CAudioGenerator(QObject* parent = nullptr);
    
    // Start audio generation
    void start();
    qint64 bytesAvailable() const override;
    
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
    
    // Get the mixed audio buffer for all harmonics
    CBuffer* getMixedBuffer();

    // Get the buffer for a specific harmonic
    CBuffer* getHarmonicBuffer(int index);
    
    // Start recording audio to a WAV file with auto generated filename
    bool startRecording();

    // Stop the current recording
    void stopRecording();

    // Check if it is already currently recording
    bool isRecording() const;
    
    // Get the path of the last recorded file
    QString getLastRecordingPath() const;

protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;

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
    
    CBuffer m_mixedBuffer;

    std::array<CBuffer, AudioConstants::NUM_HARMONICS> m_harmonicBuffers;
    std::array<SHarmonicControls, AudioConstants::NUM_HARMONICS> m_harmonics{};    

    std::array<double, AudioConstants::BUFFER_SIZE> m_mixedSamples{};
    std::array<std::array<double, AudioConstants::BUFFER_SIZE>, AudioConstants::NUM_HARMONICS> m_harmonicSamples{};
    
    CAudioRecorder m_recorder;
};

