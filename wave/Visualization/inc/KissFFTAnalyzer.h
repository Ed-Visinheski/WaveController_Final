#pragma once
#include "kiss_fft.h"
#include <vector>

/**
 * CKissFFTAnalyzer: FFT analysis wrapper using the KissFFT library
 * Computes the magnitude spectrum, finds peaks, and provides frequency conversion utils
 */
class CKissFFTAnalyzer 
{
    
public:
    explicit CKissFFTAnalyzer(size_t fftSize = 2048);

    ~CKissFFTAnalyzer();
    
    // Get the FFT size being used
    size_t getFftSize() const;

    // Convert linear magnitude to decibels
    static double magnitudeToDb(double magnitude);

    // Convert FFT index to frequency in Hz
    static double binToFrequency(size_t bin, size_t fftSize, double sampleRate);

    // Get the most recent magnitude spectrum
    const std::vector<double>& getMagnitudes() const;

    // Compute magnitude spectrum from audio samples
    const std::vector<double>& computeMagnitudeSpectrum(const std::vector<double>& samples);
    
    // Find the spectrum peaks above a threshold
    std::vector<std::pair<size_t, double>> findPeaks(double minMagnitude, size_t minSpacing) const;
    
private:
    
    size_t m_fftSize;
    kiss_fft_cfg m_cfg;
    
    std::vector<double> m_window;
    std::vector<double> m_magnitudes;
    
    std::vector<kiss_fft_cpx> m_input;
    std::vector<kiss_fft_cpx> m_output;
};