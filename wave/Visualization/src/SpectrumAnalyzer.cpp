#include "SpectrumAnalyzer.h"

CSpectrumAnalyzer::CSpectrumAnalyzer(double sampleRate, QWidget* parent)
    : QWidget(parent)
    , m_sampleRate(sampleRate)
    , m_fft(std::make_unique<CKissFFTAnalyzer>(FFT_SIZE))
    , m_updateTimer(new QTimer(this))
    , m_displayBuffer(FFT_SIZE, 0.0)
    , m_smoothingFactor(0.7)
    , m_minFreq(20.0)
    , m_maxFreq(sampleRate / 2.0)
    , m_minDb(-80.0)
    , m_maxDb(0.0)
    , m_showPeaks(true)
    , m_logScale(true)
    , m_hoverFrequency(-1.0)
{
    setMinimumSize(800, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

    connect(m_updateTimer, &QTimer::timeout, this, &CSpectrumAnalyzer::updateSpectrum);
    m_updateTimer->start(1000 / UPDATE_FPS);
}

void CSpectrumAnalyzer::updateFromSamples(const std::vector<double>& samples) 
{
    if (samples.size() < FFT_SIZE) 
    {
        return;
    }

    const auto& magnitudes = m_fft->computeMagnitudeSpectrum(samples);

    
    for (size_t i = 0; i < magnitudes.size(); ++i) 
    {
        m_displayBuffer[i] = m_displayBuffer[i] * m_smoothingFactor + magnitudes[i] * (1.0 - m_smoothingFactor);
    }
}

void CSpectrumAnalyzer::updateFromRingBuffer(CBuffer& ringBuffer) 
{
    std::vector<double> samples(FFT_SIZE);
    const size_t read = ringBuffer.peek(samples.data(), FFT_SIZE);
    
    if (read == FFT_SIZE) 
    {
        updateFromSamples(samples);
        // update();
    }
}

void CSpectrumAnalyzer::setHarmonicMarkers(double fundamental, int numHarmonics) 
{
    m_harmonics.clear();
    for (int i = 1; i <= numHarmonics; ++i) 
    {
        m_harmonics.push_back(fundamental * i);
    }
}

void CSpectrumAnalyzer::setSmoothingFactor(double factor) 
{ 
    m_smoothingFactor = std::clamp(factor, 0.0, 0.99); 
}

void CSpectrumAnalyzer::setFrequencyRange(double minFreq, double maxFreq) 
{
    m_minFreq = minFreq;
    m_maxFreq = maxFreq;
}

void CSpectrumAnalyzer::setDbRange(double minDb, double maxDb) 
{
    m_minDb = minDb;
    m_maxDb = maxDb;
}
void CSpectrumAnalyzer::setShowPeaks(bool show) 
{ 
    m_showPeaks = show; 
}

void CSpectrumAnalyzer::setLogScale(bool log) 
{ 
    m_logScale = log; 
}

void CSpectrumAnalyzer::paintEvent(QPaintEvent* event)  
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int w = width();
    const int h = height();

    painter.fillRect(rect(), QColor(20, 20, 30));

    drawFrequencyGrid(painter, w, h);
    drawSpectrum(painter, w, h);

    if (!m_harmonics.empty()) 
    {
        drawHarmonicMarkers(painter, w, h);
    }

    if (m_showPeaks) 
    {
        drawPeaks(painter, w, h);
    }

    if (m_hoverFrequency > 0) 
    {
        drawHoverInfo(painter);
    }

    drawLabels(painter, w, h);
}

void CSpectrumAnalyzer::mouseMoveEvent(QMouseEvent* event)  
{
    const int x = event->pos().x();
    const int w = width();
    
    const double normalized = static_cast<double>(x) / w;
    
    if (m_logScale) 
    {
        const double logMin = std::log10(m_minFreq);
        const double logMax = std::log10(m_maxFreq);
        m_hoverFrequency = std::pow(10.0, logMin + normalized * (logMax - logMin));
    } 
    else 
    {
        m_hoverFrequency = m_minFreq + normalized * (m_maxFreq - m_minFreq);
    }
    update();
}

void CSpectrumAnalyzer::leaveEvent(QEvent* event)  
{
    Q_UNUSED(event);
    m_hoverFrequency = -1.0;
    update();
}

void CSpectrumAnalyzer::drawFrequencyGrid(QPainter& painter, int w, int h) {
    painter.setPen(QColor(50, 50, 60));

    for (int db = static_cast<int>(m_minDb); db <= static_cast<int>(m_maxDb); db += 10) 
    {
        const double y = dbToY(db, h);
        painter.drawLine(0, y, w, y);
        
        painter.setPen(QColor(150, 150, 160));
        painter.drawText(5, y - 2, QString("%1 dB").arg(db));
        painter.setPen(QColor(50, 50, 60));
    }

    const std::vector<double> freqMarkers = getFrequencyMarkers();
    for (double freq : freqMarkers) 
    {
        const double x = frequencyToX(freq, w);
        painter.drawLine(x, 0, x, h);
    }
}

void CSpectrumAnalyzer::drawSpectrum(QPainter& painter, int w, int h) 
{
    painter.setPen(Qt::NoPen);
    
    const size_t numBins = m_displayBuffer.size();
    
    for (size_t bin = 1; bin < numBins; ++bin) 
    {
        const double freq = CKissFFTAnalyzer::binToFrequency(bin, FFT_SIZE, m_sampleRate);
        if (freq < m_minFreq || freq > m_maxFreq) 
        {
            continue;
        }

        const double magnitude = m_displayBuffer[bin];
        const double db = CKissFFTAnalyzer::magnitudeToDb(magnitude);
        
        const double x = frequencyToX(freq, w);
        const double y = dbToY(db, h);
        const double barHeight = h - y;

        const double normalizedDb = (db - m_minDb) / (m_maxDb - m_minDb);
        const QColor color = getMagnitudeColor(normalizedDb);
        
        painter.fillRect(QRectF(x, y, 2, barHeight), color);
    }
}

void CSpectrumAnalyzer::drawPeaks(QPainter& painter, int w, int h) 
{
    const auto peaks = m_fft->findPeaks(0.01, 3);    
    painter.setPen(QPen(QColor(255, 255, 0), 2));
    
    for (const auto& [bin, magnitude] : peaks) 
    {
        const double freq = CKissFFTAnalyzer::binToFrequency(bin, FFT_SIZE, m_sampleRate);
        if (freq < m_minFreq || freq > m_maxFreq) 
        {
            continue;
        }
        
        const double db = CKissFFTAnalyzer::magnitudeToDb(magnitude);

        const double x = frequencyToX(freq, w);
        const double y = dbToY(db, h);
        
        painter.drawEllipse(QPointF(x, y), 4, 4);
    }
}

void CSpectrumAnalyzer::drawHarmonicMarkers(QPainter& painter, int w, int h) 
{
    painter.setPen(QPen(QColor(255, 100, 100, 100), 1, Qt::DashLine));
    
    for (size_t i = 0; i < m_harmonics.size(); ++i) 
    {
        const double freq = m_harmonics[i];
        if (freq < m_minFreq || freq > m_maxFreq) 
        {
            continue;
        }
        
        const double x = frequencyToX(freq, w);
        painter.drawLine(x, 0, x, h);
        
        painter.setPen(QColor(255, 150, 150));
        painter.drawText(x + 3, 15, QString("H%1").arg(i + 1));
        painter.setPen(QPen(QColor(255, 100, 100, 100), 1, Qt::DashLine));
    }
}

void CSpectrumAnalyzer::drawHoverInfo(QPainter& painter) 
{
    const QString text = QString("Frequency: %1 Hz").arg(m_hoverFrequency, 0, 'f', 1);
    
    painter.setPen(Qt::white);
    painter.drawText(10, height() - 10, text);
}

void CSpectrumAnalyzer::drawLabels(QPainter& painter, int w, int h) 
{
    Q_UNUSED(h);
    
    painter.setPen(QColor(200, 200, 210));
    const std::vector<double> freqMarkers = getFrequencyMarkers();
    
    for (double freq : freqMarkers) 
    {
        QString label;
        const double x = frequencyToX(freq, w);
    
        if (freq >= 1000) 
        {
            label = QString("%1k").arg(freq / 1000.0, 0, 'f', 1);
        } 
        
        else 
        {
            label = QString("%1").arg(static_cast<int>(freq));
        }
        
        painter.drawText(x - 15, height() - 25, label);
    }
}

double CSpectrumAnalyzer::frequencyToX(double freq, int width) const 
{
    if (m_logScale) 
    {
        const double logMin = std::log10(m_minFreq);
        const double logMax = std::log10(m_maxFreq);
        const double logFreq = std::log10(freq);
    
        return ((logFreq - logMin) / (logMax - logMin)) * width;
    } 
    
    else 
    {
        return ((freq - m_minFreq) / (m_maxFreq - m_minFreq)) * width;
    }
}

double CSpectrumAnalyzer::dbToY(double db, int height) const 
{
    const double normalized = (db - m_minDb) / (m_maxDb - m_minDb);
    return height * (1.0 - normalized);
}

QColor CSpectrumAnalyzer::getMagnitudeColor(double normalized) const 
{
    // Blue -> Green -> Yellow -> Red gradient
    normalized = std::clamp(normalized, 0.0, 1.0);
    
    if (normalized < 0.33) 
    {
        // Blue to green
        const double t = normalized / 0.33;
        return QColor(0, 100 + t * 155, 255 - t * 155);
    } 
    else if (normalized < 0.66) 
    {
        // Green to yellow
        const double t = (normalized - 0.33) / 0.33;
        return QColor(t * 255, 255, 100 * (1 - t));
    } 
    else 
    {
        // Yellow to red
        const double t = (normalized - 0.66) / 0.34;
        return QColor(255, 255 * (1 - t), 0);
    }
}

std::vector<double> CSpectrumAnalyzer::getFrequencyMarkers() const 
{
    if (m_logScale) 
    {
        std::vector<double> markers;
        for (double freq = 20; freq <= m_maxFreq; freq *= 2) 
        {
            if (freq >= m_minFreq) 
            {
                markers.push_back(freq);
            }
        }
        
        return markers;
    } 
    else 
    {
        std::vector<double> markers;
        const double step = (m_maxFreq - m_minFreq) / 10;

        for (int i = 0; i <= 10; ++i) 
        {
            markers.push_back(m_minFreq + i * step);
        }

        return markers;
    }
}

void CSpectrumAnalyzer::updateSpectrum() 
{
    update();
}