#pragma once

#include <QWidget>
#include <QPainter>
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

class CWaveform;

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

    static constexpr size_t DISPLAY_SAMPLES = 2048;
    static constexpr int UPDATE_FPS = 60;

    struct WaveformChannel 
    {
        QColor color;
        std::string name;
        bool visible = true;

        std::atomic<bool> swapRequested;

        std::vector<double> backBuffer;
        std::vector<double> frontBuffer;

        
        WaveformChannel(const std::string& n, const QColor& c) 
                        : name(n)
                        , color(c)
                        , swapRequested(false)
                        , frontBuffer(DISPLAY_SAMPLES, 0.0)
                        , backBuffer(DISPLAY_SAMPLES, 0.0) {}
    };
    
    friend class CWaveformDisplay;
    explicit CWaveform(QWidget* parent = nullptr);

    size_t findTriggerPoint(const std::vector<double>& samples) const;
    
    void addChannel(const std::string& name, const QColor& color);
    void updateChannel(size_t channelIndex, const std::vector<double>& samples);
    void updateChannelFromBuffer(size_t channelIndex, CBuffer& buffer);

    
    void setYScale(double scale) { m_yScale = scale; };
    void setAutoScale(bool enable) { m_autoScale = enable; };
    void setChannelVisible(size_t index, bool visible);
    
    void setTriggerLevel(double level) { m_triggerLevel = level; };
    void setTriggerEnabled(bool enable) { m_triggerEnabled = enable; };

    void setDisplayMode(DisplayMode mode);
    DisplayMode getDisplayMode() const { return m_displayMode; }
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
    bool m_triggerEnabled;
    
    double m_yScale;
    double m_triggerLevel;
    
    DisplayMode m_displayMode;
    QTimer* m_updateTimer;
    QTabBar* m_tabBar;
    CWaveformDisplay* m_waveformWidget;
    
    std::vector<std::unique_ptr<WaveformChannel>> m_channels;
    std::vector<double> m_discardBuffer = std::vector<double>(512, 0.0);
};

