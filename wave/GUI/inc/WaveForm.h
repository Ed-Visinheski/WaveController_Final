#pragma once

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QTabBar>
#include <QVBoxLayout>
#include <QDebug>
#include <cmath>
#include <limits>
#include <vector>
#include <memory>
#include <atomic>
#include "Buffer.h"
#include "Constants.h"
#include "Timer.h"

class CWaveform;

/**
 * CWaveformDisplay: Display widget for rendering waveform graphics
 * Handles paint events and delegates drawing to the parent CWaveform class
 */
class CWaveformDisplay : public QWidget 
{
    Q_OBJECT
    
public:
    explicit CWaveformDisplay(CWaveform* parent);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    
private:
    CWaveform* m_waveform;
};

/**
 * CWaveform: Multi channel waveform visualization with three display modes
 * Displays the waveforms in Solo, Separate, or Combined view
 */
class CWaveform : public QWidget 
{
    Q_OBJECT

public:
    enum class DisplayMode 
    {
        Solo,
        Separate,
        Combined
    };

    struct WaveformChannel 
    {
        QColor color;
        std::string name;
        bool visible = true;

        // Large buffer to hold captured samples from ring buffer
        std::array<double, AudioConstants::BUFFER_SIZE> buffer;
        size_t writePos = 0;  // Current write position for new samples
        
        WaveformChannel(const std::string& n, const QColor& c) 
                        : name(n)
                        , color(c)
        {
            buffer.fill(0.0);
        }
    };
    
    friend class CWaveformDisplay;
    explicit CWaveform(QWidget* parent = nullptr);

    // Add a new waveform channel with name and color
    void addChannel(const std::string& name, const QColor& color);

    // Update channel data from the vector of samples
    void updateChannel(size_t channelIndex, const std::vector<double>& samples);
    
    // Update channel data from the buffer
    void updateChannelFromBuffer(size_t channelIndex, CBuffer<double>& buffer);

    // Set vertical scale factor
    void setYScale(double scale) { m_yScale = scale; };

    // Enable or disable automatic amplitude scaling
    void setAutoScale(bool enable) { m_autoScale = enable; };
    
    // Show or hide a specific channel
    void setChannelVisible(size_t index, bool visible);

    // Change the display mode sucvh as Solo, Separate orCombined 
    void setDisplayMode(DisplayMode mode);

    // Get the current display mode
    DisplayMode getDisplayMode() const { return m_displayMode; }
    
    // Trigger repaint of waveform display
    void refresh();
    
protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUI();
    void performPaint(QPainter& painter, int w, int h);
    void drawGrid(QPainter& painter, int width, int height);
    void drawGridCombined(QPainter& painter, int width, int height);
    void drawGridSeparate(QPainter& painter, int width, int height);
    void drawWaveform(QPainter& painter, const WaveformChannel& channel, int width, int height, size_t startIndex, size_t channelIndex, size_t numVisibleChannels);
    void drawLabels(QPainter& painter, int h);

    bool m_autoScale;
    
    double m_yScale;
    
    QTabBar* m_tabBar;
    DisplayMode m_displayMode;
    CWaveformDisplay* m_waveformWidget;
    
    std::vector<std::unique_ptr<WaveformChannel>> m_channels;
    
    // Display window position that advances each frame for smooth scrolling
    size_t m_displayWindowPos = 0;
};

