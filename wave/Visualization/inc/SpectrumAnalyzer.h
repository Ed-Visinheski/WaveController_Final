#pragma once

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <vector>
#include <memory>
#include <cmath>
#include "KissFFTAnalyzer.h"
#include "Buffer.h"
#include "Constants.h"
#include "Timer.h"

/**
 * CSpectrumAnalyzer: Frequency spectrum analyzer with FFT visualization
 * Displays magnitude spectrum with logarithmic scaling, peak detection, and harmonic markers
 */
class CSpectrumAnalyzer : public QWidget 
{
    Q_OBJECT

public:
    explicit CSpectrumAnalyzer(double sampleRate = AudioConstants::SAMPLE_RATE, QWidget* parent = nullptr);

    // Update spectrum from the vector of audio samples
    void updateFromSamples(const std::vector<double>& samples);

    // Update spectrum from the buffer
    void updateFromRingBuffer(CBuffer& ringBuffer);
    
    // Enable or disable logarithmic frequency scale
    void setLogScale(bool log);

    // Enable or disable peak frequency markers
    void setShowPeaks(bool show);

    // Set smoothing factor for spectrum display from 0.0 to 1.0
    void setSmoothingFactor(double factor);
    
    // Set the decibel range for the display
    void setDbRange(double minDb, double maxDb);
    
    // Set the frequency range to display in Hz
    void setFrequencyRange(double minFreq, double maxFreq);

    // Set harmonic markers for the fundamental frequency
    void setHarmonicMarkers(double fundamental, int numHarmonics = AudioConstants::MAX_HARMONICS);

protected:
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    
    void drawHoverInfo(QPainter& painter);
    void drawLabels(QPainter& painter, int w, int h);

    void drawPeaks(QPainter& painter, int w, int h);
    void drawSpectrum(QPainter& painter, int w, int h);

    void drawFrequencyGrid(QPainter& painter, int w, int h);
    void drawHarmonicMarkers(QPainter& painter, int w, int h);
    
    double dbToY(double db, int height) const;
    double frequencyToX(double freq, int width) const;

    QColor getMagnitudeColor(double normalized) const;
    std::vector<double> getFrequencyMarkers() const;

    bool m_showPeaks;
    bool m_logScale;
    
    double m_minDb;
    double m_maxDb;
    
    double m_minFreq;
    double m_maxFreq;
    
    double m_sampleRate;
    double m_hoverFrequency;
    double m_smoothingFactor;
    
    std::unique_ptr<CKissFFTAnalyzer> m_fft;

    std::vector<double> m_displayBuffer;
    std::vector<double> m_harmonics;

private slots:
    void updateSpectrum();
};
