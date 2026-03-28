#include "WaveForm.h"
#include "Constants.h"

CWaveformDisplay::CWaveformDisplay(CWaveform* parent)
    : QWidget(parent)
    , m_waveform(parent)
{
    setMinimumSize(800, 400);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void CWaveformDisplay::paintEvent(QPaintEvent* event) 
{
    Q_UNUSED(event);
    
    if (!m_waveform) 
    {
        return;
    }
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const int w = width();
    const int h = height();
    
    painter.fillRect(rect(), QColor(20, 20, 30));
    
    m_waveform->performPaint(painter, w, h);
}

// CWaveform implementation
CWaveform::CWaveform(QWidget* parent)
        : QWidget(parent)
        , m_updateTimer(new QTimer(this))
        , m_autoScale(true)
        , m_yScale(1.0)
        , m_triggerLevel(0.0)
        , m_triggerEnabled(false)
        , m_displayMode(DisplayMode::Separate)
        , m_tabBar(nullptr)
        , m_waveformWidget(nullptr)
{
    m_channels.reserve(AudioConstants::MAX_HARMONICS); 

    setupUI();

    connect(m_updateTimer, &QTimer::timeout, this, [this]() 
    {
        if (m_waveformWidget) {
            m_waveformWidget->update();
        }
    });
    m_updateTimer->start(1000 / UPDATE_FPS);

    addChannel("Mixed", QColor(0, 255, 128));
}

void CWaveform::setupUI() 
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabBar = new QTabBar(this);
    m_tabBar->addTab("Solo");
    m_tabBar->addTab("Separate");
    m_tabBar->addTab("Combined");
    m_tabBar->setCurrentIndex(1);
    m_tabBar->setExpanding(false);
    
    m_tabBar->setStyleSheet
    (
        "QTabBar { background: transparent; }"
        "QTabBar::tab { "
        "  background: #2a2a2a; "
        "  color: #cccccc; "
        "  padding: 4px 12px; "
        "  border: 1px solid #404040; "
        "  border-bottom: none; "
        "}"
        "QTabBar::tab:selected { "
        "  background: #3a3a3a; "
        "  color: white; "
        "}"
        "QTabBar::tab:hover { "
        "  background: #353535; "
        "}"
    );

    connect(m_tabBar, &QTabBar::currentChanged, this, [this](int index) 
    {
        setDisplayMode(static_cast<DisplayMode>(index));
    });

    m_waveformWidget = new CWaveformDisplay(this);
    auto* tabContainer = new QWidget(this);
    auto* tabLayout = new QHBoxLayout(tabContainer);

    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->addStretch();
    tabLayout->addWidget(m_tabBar);
    
    layout->addWidget(tabContainer);
    layout->addWidget(m_waveformWidget, 1);
}

void CWaveform::setDisplayMode(DisplayMode mode) 
{
    m_displayMode = mode;
    if (m_waveformWidget) 
    {
        m_waveformWidget->update();
    }
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

void CWaveform::resizeEvent(QResizeEvent* event) 
{
    QWidget::resizeEvent(event);
    if (m_waveformWidget) 
    {
        m_waveformWidget->update();
    }
}

void CWaveform::performPaint(QPainter& painter, int w, int h) 
{
    for (auto& channel : m_channels) 
    {
        if (channel->swapRequested.load(std::memory_order_acquire)) 
        {
            std::swap(channel->frontBuffer, channel->backBuffer);
            channel->swapRequested.store(false, std::memory_order_release);
        }
    }

    drawGrid(painter, w, h);

    size_t startIndex = 0;
    if (m_triggerEnabled && !m_channels.empty()) 
    {
        startIndex = findTriggerPoint(m_channels[0]->frontBuffer);
    }

    if (m_displayMode == DisplayMode::Solo) 
    {
        if (!m_channels.empty() && m_channels[0]->visible) 
        {
            drawWaveform(painter, *m_channels[0], w, h, startIndex, 0, 1);
        }
    }
    else if (m_displayMode == DisplayMode::Combined) 
    {
        for (const auto& channel : m_channels) 
        {
            if (channel->visible) 
            {
                drawWaveform(painter, *channel, w, h, startIndex, 0, 1);
            }
        }
    }
    else 
    {
        size_t visibleChannelIndex = 0;
        size_t numVisibleChannels = 0;
        for (const auto& channel : m_channels) 
        {
            if (channel->visible) ++numVisibleChannels;
        }
        
        for (const auto& channel : m_channels) 
        {
            if (channel->visible) 
            {
                drawWaveform(painter, *channel, w, h, startIndex, visibleChannelIndex, numVisibleChannels);
                ++visibleChannelIndex;
            }
        }
    }

    drawLabels(painter, h);
}


void CWaveform::drawGrid(QPainter& painter, int w, int h) 
{
    if (m_displayMode == DisplayMode::Separate) 
    {
        drawGridSeparate(painter, w, h);
    }
    else 
    {
        drawGridCombined(painter, w, h);
    }
}

void CWaveform::drawGridCombined(QPainter& painter, int w, int h) 
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

void CWaveform::drawGridSeparate(QPainter& painter, int w, int h) 
{
    size_t numVisibleChannels = 0;
    for (const auto& channel : m_channels) 
    {
        if (channel->visible) 
        {
            ++numVisibleChannels;
        }
    }
    
    if (numVisibleChannels == 0) 
    {
        return;
    }
    
    const double sectionHeight = h / static_cast<double>(numVisibleChannels);
    
    painter.setPen(QColor(50, 50, 60));

    for (size_t i = 0; i <= numVisibleChannels; ++i) 
    {
        int y = static_cast<int>(i * sectionHeight);
        painter.drawLine(0, y, w, y);
    }

    for (int i = 0; i <= 8; ++i) 
    {
        int x = w * i / 8;
        painter.drawLine(x, 0, x, h);
    }

    painter.setPen(QColor(80, 80, 90));
    for (size_t i = 0; i < numVisibleChannels; ++i) 
    {
        int centerY = static_cast<int>((i + 0.5) * sectionHeight);
        painter.drawLine(0, centerY, w, centerY);
    }
}

void CWaveform::drawWaveform(QPainter& painter, const WaveformChannel& channel, int w, int h, size_t startIndex, size_t channelIndex, size_t numVisibleChannels) 
{
    if (channel.frontBuffer.empty() || numVisibleChannels == 0) 
    {
        return;
    }

    const double sectionHeight = h / static_cast<double>(numVisibleChannels);
    const double sectionCenter = (channelIndex + 0.5) * sectionHeight;
    const double sectionAmplitude = sectionHeight * 0.45;

    double maxVal = m_yScale;
    if (m_autoScale) 
    {
        maxVal = 0.0;
        for (double sample : channel.frontBuffer)
        {
            maxVal = std::max(maxVal, std::abs(sample));
        }
        if (maxVal < 0.0001) //DO NOT REMOVE !!! - needed to prevent division by 0 error
        {
            maxVal = 1.0;
        }
    }

    painter.setPen(QPen(channel.color, 2.0));

    const size_t count = std::min(channel.frontBuffer.size(), DISPLAY_SAMPLES);
    for (size_t i = 1; i < count; ++i) 
    {
        const size_t index1 = (startIndex + i - 1) % count;
        const size_t index2 = (startIndex + i) % count;

        const double x1 = (i - 1) * w / static_cast<double>(count);
        const double y1 = sectionCenter - (channel.frontBuffer[index1] / maxVal) * sectionAmplitude;

        const double x2 = i * w / static_cast<double>(count);
        const double y2 = sectionCenter - (channel.frontBuffer[index2] / maxVal) * sectionAmplitude;

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

void CWaveform::drawLabels(QPainter& painter, int h) 
{
    painter.setPen(Qt::white);
    
    if (m_displayMode == DisplayMode::Solo) 
    {
        // Only show mixed channel
        if (!m_channels.empty() && m_channels[0]->visible) 
        {
            painter.fillRect(10, 10, 15, 15, m_channels[0]->color);
            painter.drawText(30, 20, QString::fromStdString(m_channels[0]->name));
        }
    }
    else if (m_displayMode == DisplayMode::Combined) 
    {
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
    else // Separate mode handling
    {
        size_t numVisibleChannels = 0;
        for (const auto& channel : m_channels) 
        {
            if (channel->visible) ++numVisibleChannels;
        }
        
        if (numVisibleChannels == 0) 
        {
            return;
        }
        
        const double sectionHeight = h / static_cast<double>(numVisibleChannels);
        size_t visibleIndex = 0;
        
        for (const auto& channel : m_channels) 
        {
            if (channel->visible) 
            {
                const int labelY = static_cast<int>((visibleIndex + 0.1) * sectionHeight + 15);
                
                painter.fillRect(10, labelY - 10, 15, 15, channel->color);
                painter.drawText(30, labelY, QString::fromStdString(channel->name));
                
                ++visibleIndex;
            }
        }
    }
}
