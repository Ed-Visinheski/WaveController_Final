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


class CSpectrumAnalyzer : public QWidget 
{
    Q_OBJECT

public:
    explicit CSpectrumAnalyzer(double sampleRate = AudioConstants::SAMPLE_RATE, QWidget* parent = nullptr);

    void updateFromSamples(const std::vector<double>& samples);

    void updateFromRingBuffer(CBuffer& ringBuffer);
    
    void setLogScale(bool log);
    void setShowPeaks(bool show);

    void setSmoothingFactor(double factor);
    void setDbRange(double minDb, double maxDb);
    
    void setFrequencyRange(double minFreq, double maxFreq);
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
