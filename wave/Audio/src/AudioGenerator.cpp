#include "AudioGenerator.h"

CAudioGenerator::CAudioGenerator(CAudioConsumer& audioConsumer, QObject* parent) : QObject(parent), m_audioConsumer(&audioConsumer) 
{
    connectSignals();
    m_reverbBuffer.resize(m_sampleRate / 4, 0.0);
}

void CAudioGenerator::connectSignals()
{
    connect(m_audioConsumer, &CAudioConsumer::generateAudio, this, &CAudioGenerator::generateAudio);
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

CBuffer<double>* CAudioGenerator::getHarmonicBuffer(int index) 
{
    if (index >= 0 && index < m_numHarmonics) 
    {
        return &m_harmonicBuffers[index];
    }
    return nullptr;
}

std::array<CBuffer<double>, AudioConstants::NUM_HARMONICS>* CAudioGenerator::getHarmonicBuffers() 
{
    return &m_harmonicBuffers;
}

CBuffer<int16_t>* CAudioGenerator::getAudioOutputBuffer() 
{
    return &m_audioOutputBuffer;
}

CBuffer<double>* CAudioGenerator::getMixedBuffer() 
{
    return &m_mixedBuffer;
}

void CAudioGenerator::generateAudio()
{
    int numOfSamples = m_audioOutputBuffer.getAvailableWrite();

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
            
            double currentSample = 0.0;
            
            if (currentHarmonic.enabled.load(std::memory_order_relaxed)) 
            {
                const double amplitude = currentHarmonic.amplitude.load(std::memory_order_relaxed);
                const double phaseOffset = currentHarmonic.phase.load(std::memory_order_relaxed);
                
                const double phase = m_currentPhase * (harmonicIndex + 1) + phaseOffset;
                currentSample = std::sin(2.0 * M_PI * phase) * amplitude;
                mixedSample += currentSample;
            }
            
            // Write harmonic to buffer (harmonic index 0-7)
            m_harmonicBuffers[harmonicIndex].write(&currentSample, 1);
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
        
        // Apply final soft clipping
        mixedSample = std::tanh(mixedSample * 1.2);
        
        // Write double mixed signal for visualization
        m_mixedBuffer.write(&mixedSample, 1);
        
        // Convert to int16_t for audio output
        int16_t audioOutput = static_cast<int16_t>(mixedSample * 32767.0 * 0.8);
        m_audioOutputBuffer.write(&audioOutput, 1);
    }
}

