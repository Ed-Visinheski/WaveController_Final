#include "KissFFTAnalyzer.h"

CKissFFTAnalyzer::CKissFFTAnalyzer(size_t fftSize)
        : m_fftSize(fftSize)
        , m_window(fftSize)
        , m_magnitudes(fftSize / 2)
        , m_input(fftSize)
        , m_output(fftSize)
{
    m_cfg = kiss_fft_alloc(fftSize, 0, nullptr, nullptr);
    
    for (size_t i = 0; i < fftSize; ++i) 
    {
        m_window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (fftSize - 1)));
    }
};

CKissFFTAnalyzer::~CKissFFTAnalyzer() 
{
    kiss_fft_free(m_cfg);
};

const std::vector<double>& CKissFFTAnalyzer::computeMagnitudeSpectrum(const std::vector<double>& samples) 
{
    const size_t N = std::min(samples.size(), m_fftSize);
    
    for (size_t i = 0; i < N; ++i) 
    {
        m_input[i].r = samples[i] * m_window[i];
        m_input[i].i = 0.0;
    }
    for (size_t i = N; i < m_fftSize; ++i) 
    {
        m_input[i].r = 0.0;
        m_input[i].i = 0.0;
    }
    
    kiss_fft(m_cfg, m_input.data(), m_output.data());
    
    for (size_t i = 0; i < m_fftSize / 2; ++i) 
    {
        const double real = m_output[i].r;
        const double imag = m_output[i].i;
        m_magnitudes[i] = std::sqrt(real * real + imag * imag) / N;
    }
    
    return m_magnitudes;
};

const std::vector<double>& CKissFFTAnalyzer::getMagnitudes() const 
{ 
    return m_magnitudes; 
}

size_t CKissFFTAnalyzer::getFftSize() const 
{ 
    return m_fftSize; 
}

double CKissFFTAnalyzer::magnitudeToDb(double magnitude) 
{
    constexpr double minDb = -100.0;
    if (magnitude < 1e-10) return minDb;
    return std::max(minDb, 20.0 * std::log10(magnitude));
};

double CKissFFTAnalyzer::binToFrequency(size_t bin, size_t fftSize, double sampleRate) 
{
    return (bin * sampleRate) / fftSize;
};

std::vector<std::pair<size_t, double>> CKissFFTAnalyzer::findPeaks(double minMagnitude = 0.01, size_t minSpacing = 5) const 
{
    std::vector<std::pair<size_t, double>> peaks;
    
    for (size_t i = minSpacing; i < m_magnitudes.size() - minSpacing; ++i) 
    {
        bool isPeak = true;

        if (m_magnitudes[i] < minMagnitude)
        {
            continue;
        }

        for (size_t j = i - minSpacing; j <= i + minSpacing; ++j) 
        {
            if (j != i && m_magnitudes[j] >= m_magnitudes[i]) 
            {
                isPeak = false;
                break;
            }
        }
        
        if (isPeak) 
        {
            peaks.push_back({i, m_magnitudes[i]});
        }
    }   
    return peaks;
}