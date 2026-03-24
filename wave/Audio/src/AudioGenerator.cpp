#include "AudioGenerator.h"


    CAudioGenerator::CAudioGenerator(QObject* parent) : QIODevice(parent)
    {
        m_reverbBuffer.resize(m_sampleRate / 4, 0.0);
    }

    void CAudioGenerator::start() 
    {
        open(QIODevice::ReadOnly);
    }

    qint64 CAudioGenerator::bytesAvailable() const 
    {
        return AudioConstants::BUFFER_SIZE * 2 + QIODevice::bytesAvailable();
    }
    
    void CAudioGenerator::setFundamental(double frequency) 
    {
        m_fundamental.store(frequency, std::memory_order_relaxed);
    }

    void CAudioGenerator::setMasterAmplitude(double amplitude) 
    {
        m_masterAmplitude.store(amplitude, std::memory_order_relaxed);
    }

    void CAudioGenerator::setHarmonicAmplitude(int harmonicIndex, double amplitude) 
    {
        if (harmonicIndex >= 0 && harmonicIndex < m_numHarmonics) 
        {
            m_harmonics[harmonicIndex].amplitude.store(amplitude, std::memory_order_relaxed);
        }
    }

    void CAudioGenerator::setHarmonicPhase(int harmonicIndex, double phase) 
    {
        if (harmonicIndex >= 0 && harmonicIndex < m_numHarmonics) 
        {
            m_harmonics[harmonicIndex].phase.store(phase, std::memory_order_relaxed);
        }
    }

    void CAudioGenerator::setHarmonicEnabled(int harmonicIndex, bool enabled) 
    {
        if (harmonicIndex >= 0 && harmonicIndex < m_numHarmonics) 
        {
            m_harmonics[harmonicIndex].enabled.store(enabled, std::memory_order_relaxed);
        }
    }

    void CAudioGenerator::setDistortion(double distortion) 
    {
        m_distortion.store(distortion, std::memory_order_relaxed);
    }

    void CAudioGenerator::setFilter(double filter) 
    {
        m_filter.store(filter, std::memory_order_relaxed);
    }

    void CAudioGenerator::setReverbMix(double reverb) 
    {
        m_reverbMix.store(reverb, std::memory_order_relaxed);
    }
    
    CBuffer* CAudioGenerator::getHarmonicBuffer(int index) 
    {
        if (index >= 0 && index < m_numHarmonics) 
        {
            return &m_harmonicBuffers[index];
        }
        return nullptr;
    }

    CBuffer* CAudioGenerator::getMixedBuffer() 
    {
        return &m_mixedBuffer;
    }

    qint64 CAudioGenerator::readData(char* data, qint64 len)
    {
        qint64 total {0};
        int16_t* qtAudioSampleBuffer = reinterpret_cast<int16_t*>(data);
        int numOfSamples = len / 2;

        const double filter = m_filter.load(std::memory_order_relaxed);
        const double reverbMix = m_reverbMix.load(std::memory_order_relaxed);
        const double distortion = m_distortion.load(std::memory_order_relaxed);

        const double fundamental = m_fundamental.load(std::memory_order_relaxed);
        const double masterAmp = m_masterAmplitude.load(std::memory_order_relaxed);

        for (int sampleIndex {0}; sampleIndex < numOfSamples; ++sampleIndex) 
        {
            double mixedSample = 0.0;

            for (int harmonicIndex {0}; harmonicIndex < m_numHarmonics; ++harmonicIndex) 
            {
                const auto& currentHarmonic = m_harmonics[harmonicIndex];
                
                if (!currentHarmonic.enabled.load(std::memory_order_relaxed)) 
                {
                    m_harmonicSamples[harmonicIndex][sampleIndex] = 0.0;
                    continue;
                }

                const double amplitude = currentHarmonic.amplitude.load(std::memory_order_relaxed);
                const double phaseOffset = currentHarmonic.phase.load(std::memory_order_relaxed);
                
                const double phase = m_currentPhase * (harmonicIndex + 1) + phaseOffset;
                const double currentSample = std::sin(2.0 * M_PI * phase) * amplitude;
                
                m_harmonicSamples[harmonicIndex][sampleIndex] = currentSample;
                mixedSample += currentSample;
            }

            m_currentPhase += fundamental / m_sampleRate;
            if (m_currentPhase >= 1.0) 
            {
                m_currentPhase -= 1.0;
            }

            if (distortion > 0.0) 
            {
                mixedSample = std::tanh(mixedSample * (1.0 + distortion * 8.0)) * 0.8;
            }

            m_lastSample = m_lastSample * (1.0 - filter) + mixedSample * filter;
            mixedSample = m_lastSample;

            if (reverbMix > 0.0) 
            {
                double reverbSample = m_reverbBuffer[m_reverbIndex];
                m_reverbBuffer[m_reverbIndex] = mixedSample + reverbSample * 0.5;
                m_reverbIndex = (m_reverbIndex + 1) % m_reverbBuffer.size();
                mixedSample = mixedSample * (1.0 - reverbMix) + reverbSample * reverbMix;
            }

            mixedSample *= masterAmp;
            m_mixedSamples[sampleIndex] = mixedSample;

            const double clipped = std::tanh(mixedSample * 1.2);
            qtAudioSampleBuffer[sampleIndex] = static_cast<int16_t>(clipped * 32767.0 * 0.8);
            total += 2;
        }

        for (int harmonicIndex = 0; harmonicIndex < m_numHarmonics; ++harmonicIndex) 
        {
            m_harmonicBuffers[harmonicIndex].write(m_harmonicSamples[harmonicIndex].data(), numOfSamples);
        }
        m_mixedBuffer.write(m_mixedSamples.data(), numOfSamples);

        return total;
    }

    qint64 CAudioGenerator::writeData(const char* data, qint64 len)
    {
        Q_UNUSED(data);
        Q_UNUSED(len);
        return 0;
    }

