#include "SynthVisualizationWindow.h"
#include "AudioConsumer.h"
// #include "AudioGenerator.h"

CSynthVisualizationWindow::CSynthVisualizationWindow(CAudioConsumer* audioConsumer, CAudioGenerator* audioGenerator, int numHarmonics, QWidget* parent)
    : QMainWindow(parent)
    , m_audioConsumer(audioConsumer)
    , m_audioGenerator(audioGenerator)
    , m_numHarmonics(numHarmonics)
    , m_activeHarmonicCount(numHarmonics)
{
    setWindowTitle("Wave - Virtual Synthesizer with Hand Tracking");
    resize(1200, 900);
    
    setupUI();
    connectSignals();
    startVisualization();
}

void CSynthVisualizationWindow::setupUI() 
{
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

    auto* centralWidget = new QWidget();
    scrollArea->setWidget(centralWidget);

    auto* mainLayout = new QVBoxLayout(centralWidget);

    auto* recordingControlsLayout = new QHBoxLayout();
    recordingControlsLayout->addStretch();
    
    m_recordButton = new QPushButton("Record");
    m_recordButton->setCheckable(true);
    m_recordButton->setStyleSheet
    (
        "QPushButton { padding: 8px 16px; font-size: 14px; font-weight: bold; }"
        "QPushButton:checked { background-color: #ff4444; color: white; }"
    );

    connect(m_recordButton, &QPushButton::clicked, this, &CSynthVisualizationWindow::toggleRecording);
    
    m_recordingStatusLabel = new QLabel("");
    m_recordingStatusLabel->setStyleSheet("font-size: 12px;");
    
    recordingControlsLayout->addWidget(m_recordButton);
    recordingControlsLayout->addWidget(m_recordingStatusLabel);
    recordingControlsLayout->addStretch();
    
    mainLayout->addLayout(recordingControlsLayout);
    
    auto* splitter = new QSplitter(Qt::Vertical);
    
    auto* waveformBox = new QGroupBox("Time Domain");
    auto* waveformLayout = new QVBoxLayout(waveformBox);
    
    // m_waveformView = new CWaveform();
    
    const QColor harmonicColors[] = 
    {
        QColor(255, 100, 100),  // H1 == Red
        QColor(255, 200, 100),  // H2 == Orange
        QColor(255, 255, 100),  // H3 == Yellow
        QColor(100, 255, 100),  // H4 == Green
        QColor(100, 255, 255),  // H5 == Cyan
        QColor(100, 150, 255),  // H6 == Light Blue
        QColor(150, 100, 255),  // H7 == Purple
        QColor(255, 100, 255),  // H8 == Magenta
    };
    
    for (int i = 0; i < m_numHarmonics; ++i) 
    {
        const QString name = QString("H%1 (%2 Hz)").arg(i + 1).arg(440 * (i + 1));
        m_waveformView.addChannel(name.toStdString(), harmonicColors[i % AudioConstants::MAX_HARMONICS]);
    }

    
    waveformLayout->addWidget(&m_waveformView);
    splitter->addWidget(waveformBox);

    auto* spectrumBox = new QGroupBox("Frequency Domain");
    auto* spectrumLayout = new QVBoxLayout(spectrumBox);
    
    // m_spectrumAnalyzer = new CSpectrumAnalyzer(44100);
    m_spectrumAnalyzer.setLogScale(true);
    m_spectrumAnalyzer.setShowPeaks(true);
    m_spectrumAnalyzer.setFrequencyRange(20, 8000);
    
    spectrumLayout->addWidget(&m_spectrumAnalyzer);
    splitter->addWidget(spectrumBox);
        
    auto* controlBox = new QGroupBox("Harmonic Controls");
    auto* controlLayout = new QVBoxLayout(controlBox);
    
    // m_harmonicPanel = new CHarmonicControlPanel(m_numHarmonics);
    controlLayout->addWidget(&m_harmonicPanel);
    
    splitter->addWidget(controlBox);

    splitter->setStretchFactor(0, 4);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 3);
    
    mainLayout->addWidget(splitter);

    m_audioConsumer->setWaveform(m_waveformView);
    m_audioConsumer->setSpectrumAnalyzer(m_spectrumAnalyzer);
    m_audioConsumer->setHarmonicControlPanel(m_harmonicPanel);

    // Set buffers: audioOutputBuffer (int16_t), mixedBuffer (double), harmonicBuffers (double array)
    m_audioConsumer->setAudioOutputBuffer(*m_audioGenerator->getAudioOutputBuffer());
    m_audioConsumer->setMixedBuffer(*m_audioGenerator->getMixedBuffer());
    m_audioConsumer->setHarmonicBuffers(*m_audioGenerator->getHarmonicBuffers());
}

void CSynthVisualizationWindow::connectSignals() 
{
    // Audio consumption signal ensures GUI stays synchronized with playback
    // Queued connection since it comes from audio thread
    connect(m_audioConsumer, &CAudioConsumer::requestGuiUpdate, this, &CSynthVisualizationWindow::updateVisualizations, Qt::QueuedConnection);
    
    connect(&m_harmonicPanel, &CHarmonicControlPanel::parametersChanged, this, &CSynthVisualizationWindow::updateAudioParameters);
    connect(&m_harmonicPanel, &CHarmonicControlPanel::harmonicChanged, this, [this](int harmonic) 
    {
        Q_UNUSED(harmonic);
        updateAudioParameters();
    });
    connect(&m_harmonicPanel, &CHarmonicControlPanel::harmonicCountChanged, this, &CSynthVisualizationWindow::updateActiveHarmonics);
}

void CSynthVisualizationWindow::startVisualization() 
{
    //This timer may be required later ot separate Audio and GUI Threads - however its not hooked up to anything currently
    connect(&CTimer::instance(), &CTimer::timeout, this, &CSynthVisualizationWindow::updateVisualizations);
    CTimer::instance().start();
}

void CSynthVisualizationWindow::updateAudioParameters() 
{
    if (!m_audioGenerator) 
    {
        return;
    }
    
    for (int i = 0; i < m_numHarmonics; ++i) 
    {
        const double amplitude = m_harmonicPanel.getHarmonicAmplitude(i + 1);
        const double phase = m_harmonicPanel.getHarmonicPhase(i + 1);
        
        const bool withinActiveCount = (i < m_activeHarmonicCount);
        
        m_audioGenerator->setHarmonicAmplitude(i, amplitude);
        m_audioGenerator->setHarmonicPhase(i, phase);
        m_audioGenerator->setHarmonicEnabled(i, withinActiveCount && amplitude > 0.0);
    }
}

void CSynthVisualizationWindow::updateActiveHarmonics(int count) 
{
    m_activeHarmonicCount = count;
    
    for (int i = 0; i < m_numHarmonics; ++i) 
    {
        const bool withinActiveCount = (i < m_activeHarmonicCount);
        const bool hasAmplitude = m_harmonicPanel.getHarmonicAmplitude(i + 1) > 0.01;
        m_waveformView.setChannelVisible(i + 1, withinActiveCount && hasAmplitude);
        
        m_audioGenerator->setHarmonicEnabled(i, withinActiveCount && hasAmplitude);
    }
}

void CSynthVisualizationWindow::updateVisualizations() 
{
    // Update spectrum analyzer and mixed waveform from separate mixed buffer
    m_spectrumAnalyzer.updateFromRingBuffer(*m_audioGenerator->getMixedBuffer());
    m_waveformView.updateChannelFromBuffer(0, *m_audioGenerator->getMixedBuffer());
    
    // Update individual harmonic waveforms (indices 0-7)
    for (int i = 0; i < m_numHarmonics; ++i) 
    {
        const bool withinActiveCount = (i < m_activeHarmonicCount);
        const bool hasAmplitude = m_harmonicPanel.getHarmonicAmplitude(i + 1) > 0.01;

        // Harmonic i (0-7) is now directly at harmonicBuffers[i]
        m_waveformView.updateChannelFromBuffer(i + 1, *m_audioGenerator->getHarmonicBuffer(i));
        m_waveformView.setChannelVisible(i + 1, withinActiveCount && hasAmplitude);
    }
    
    // Trigger widget repaints
    m_spectrumAnalyzer.update();
    m_waveformView.refresh();
}

void CSynthVisualizationWindow::setFundamental(double frequency) 
{

        m_audioGenerator->setFundamental(frequency);
        m_harmonicPanel.setFundamental(frequency);
        m_spectrumAnalyzer.setHarmonicMarkers(frequency, m_numHarmonics);
}

void CSynthVisualizationWindow::setMasterAmplitude(double amplitude) {
    if (m_audioGenerator) 
    {
        m_audioGenerator->setMasterAmplitude(amplitude);
    }
}

void CSynthVisualizationWindow::setEffects(double distortion, double filter, double reverb) 
{
    if (m_audioGenerator) {
        m_audioGenerator->setDistortion(distortion);
        m_audioGenerator->setFilter(filter);
        m_audioGenerator->setReverbMix(reverb);
    }
}

void CSynthVisualizationWindow::toggleRecording() 
{
    if (!m_audioConsumer) 
    {
        return;
    }
    
    if (m_audioConsumer->isRecording()) 
    {
        m_audioConsumer->stopRecording();
        m_recordButton->setText("Record");
        m_recordButton->setChecked(false);
        
        QString savedPath = m_audioConsumer->getLastRecordingPath();
        m_recordingStatusLabel->setText(QString("Saved: %1").arg(savedPath));
        
    } 

    else 
    {
        if (m_audioConsumer->startRecording()) 
        {
            m_recordButton->setChecked(true);
            m_recordButton->setText("Stop Recording");
            m_recordingStatusLabel->setText("Recording");
        } 
        else 
        {
            m_recordButton->setChecked(false);
            m_recordingStatusLabel->setText("Failed to start recording");
        }
    }
}