#pragma once
#include "kiss_fft.h"
#include <vector>

class CKissFFTAnalyzer 
{
    
public:
    explicit CKissFFTAnalyzer(size_t fftSize = 2048);

    ~CKissFFTAnalyzer();
    
    size_t getFftSize() const;

    static double magnitudeToDb(double magnitude);
    static double binToFrequency(size_t bin, size_t fftSize, double sampleRate);

    const std::vector<double>& getMagnitudes() const;
    const std::vector<double>& computeMagnitudeSpectrum(const std::vector<double>& samples);
    std::vector<std::pair<size_t, double>> findPeaks(double minMagnitude, size_t minSpacing) const;
    
private:
    
    size_t m_fftSize;
    kiss_fft_cfg m_cfg;
    
    std::vector<double> m_window;
    std::vector<double> m_magnitudes;
    
    std::vector<kiss_fft_cpx> m_input;
    std::vector<kiss_fft_cpx> m_output;
};