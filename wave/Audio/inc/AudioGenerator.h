#pragma once

#include <QIODevice>
#include <array>
#include <vector>
#include <atomic>
#include <cmath>
#include <memory>
#include "Buffer.h"
#include "Constants.h"

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
    
    void start();
    qint64 bytesAvailable() const override;
    
    void setFundamental(double frequency);
    void setMasterAmplitude(double amplitude);

    void setHarmonicAmplitude(int harmonicIndex, double amplitude);
    void setHarmonicPhase(int harmonicIndex, double phase);
    void setHarmonicEnabled(int harmonicIndex, bool enabled);

    void setDistortion(double distortion);
    void setFilter(double filter);
    void setReverbMix(double reverb);
    
    CBuffer* getMixedBuffer();
    CBuffer* getHarmonicBuffer(int index);

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
};

