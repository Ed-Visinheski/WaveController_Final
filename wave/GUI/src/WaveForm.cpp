#include "WaveForm.h"

CWaveform::CWaveform(QWidget* parent)
        : QWidget(parent)
        , m_updateTimer(new QTimer(this))
        , m_autoScale(true)
        , m_yScale(1.0)
        , m_triggerLevel(0.0)
        , m_triggerEnabled(false)
{
    setMinimumSize(800, 400);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_channels.reserve(16); 

    connect(m_updateTimer, &QTimer::timeout, this, [this]() 
    {
        update();
    });
    m_updateTimer->start(1000 / UPDATE_FPS);

    addChannel("Mixed", QColor(0, 255, 128));
}

void CWaveform::addChannel(const std::string& name, const QColor& color) 
{
    m_channels.push_back(std::make_unique<WaveformChannel>(name, color)); 
}

void CWaveform::updateChannel(size_t channelIndex, const std::vector<double>& samples) 
{
    if (channelIndex >= m_channels.size()) return;
    
    const size_t count = std::min(samples.size(), DISPLAY_SAMPLES);
    std::copy(samples.begin(), samples.begin() + count, m_channels[channelIndex]->backBuffer.begin());
    
    m_channels[channelIndex]->swapRequested.store(true, std::memory_order_release);
}

void CWaveform::updateChannelFromBuffer(size_t channelIndex, CBuffer& buffer) 
{
    if (channelIndex >= m_channels.size()) 
    {
        return;
    }
    
    size_t availableRead = buffer.getAvailableRead();

    if (availableRead == 0) 
    {
        return;
    }

    while (availableRead > DISPLAY_SAMPLES) 
    {
        const size_t drop = std::min(availableRead - DISPLAY_SAMPLES, m_discardBuffer.size());
        buffer.read(m_discardBuffer.data(), drop);
        availableRead -= drop;
    }

    buffer.read(m_channels[channelIndex]->backBuffer.data(), std::min(availableRead, DISPLAY_SAMPLES));
    m_channels[channelIndex]->swapRequested.store(true, std::memory_order_release);
}

void CWaveform::setChannelVisible(size_t index, bool visible) 
{
    if (index < m_channels.size()) 
    {
        m_channels[index]->visible = visible;
    }
}

void CWaveform::paintEvent(QPaintEvent* event) 
{
    
    Q_UNUSED(event);

    for (auto& channel : m_channels) 
    {
        if (channel->swapRequested.load(std::memory_order_acquire)) 
        {
            std::swap(channel->frontBuffer, channel->backBuffer);
            channel->swapRequested.store(false, std::memory_order_release);
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int w = width();
    const int h = height();

    painter.fillRect(rect(), QColor(20, 20, 30));

    drawGrid(painter, w, h);

    size_t startIndex = 0;
    if (m_triggerEnabled && !m_channels.empty()) 
    {
        startIndex = findTriggerPoint(m_channels[0]->frontBuffer);
    }

    for (const auto& channel : m_channels) 
    {
        if (channel->visible) 
        {
            drawWaveform(painter, *channel, w, h, startIndex);
        }
    }

    drawLabels(painter);
}


void CWaveform::drawGrid(QPainter& painter, int w, int h) 
{
    painter.setPen(QColor(50, 50, 60));

    // Horizontal lines
    for (int i = 0; i <= 4; ++i) 
    {
        int y = h * i / 4;
        painter.drawLine(0, y, w, y);
    }

    // Vertical lines
    for (int i = 0; i <= 8; ++i) 
    {
        int x = w * i / 8;
        painter.drawLine(x, 0, x, h);
    }

    // Center line!!!!
    painter.setPen(QColor(80, 80, 90));
    painter.drawLine(0, h / 2, w, h / 2);
}

void CWaveform::drawWaveform(QPainter& painter, const WaveformChannel& channel, int w, int h, size_t startIndex) 
{
    if (channel.frontBuffer.empty()) 
    {
        return;
    }

    double maxVal = m_yScale;
    if (m_autoScale) 
    {
        maxVal = 0.0;
        for (double sample : channel.frontBuffer)
        {
            maxVal = std::max(maxVal, std::abs(sample));
        }
    }

    painter.setPen(QPen(channel.color, 2.0));

    const size_t count = std::min(channel.frontBuffer.size(), DISPLAY_SAMPLES);
    for (size_t i = 1; i < count; ++i) 
    {
        const size_t idx1 = (startIndex + i - 1) % count;
        const size_t idx2 = (startIndex + i) % count;

        const double x1 = (i - 1) * w / static_cast<double>(count);
        const double y1 = h / 2 - (channel.frontBuffer[idx1] / maxVal) * h * 0.4;

        const double x2 = i * w / static_cast<double>(count);
        const double y2 = h / 2 - (channel.frontBuffer[idx2] / maxVal) * h * 0.4;

        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }
}

size_t CWaveform::findTriggerPoint(const std::vector<double>& samples) const 
{
    for (size_t i = 1; i < samples.size() / 2; ++i) 
    {
        if (samples[i - 1] <= m_triggerLevel && samples[i] > m_triggerLevel) 
        {
            return i;
        }
    }
    return 0;
}

void CWaveform::drawLabels(QPainter& painter) 
{
    painter.setPen(Qt::white);

    int y = 20;
    for (const auto& channel : m_channels) 
    {
        if (channel->visible) 
        {
            painter.fillRect(10, y - 10, 15, 15, channel->color);
            painter.drawText(30, y, QString::fromStdString(channel->name));
            y += 20;
        }
    }
}
