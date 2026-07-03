#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStatusBar>
#include <QTimer>
#include <QDebug>
#include "WaveForm.h"
#include "HarmonicControlPanel.h"
#include "AudioGenerator.h"
#include "Constants.h"
#include "SpectrumAnalyzer.h"
#include "AudioConsumer.h"
#include "Timer.h"

/**
 * CSynthVisualizationWindow: Main application window integratingthe  waveform, spectrum, and harmonic controls
 * Manages audio generation, visualization updates, and UI components
 */
class CSynthVisualizationWindow : public QMainWindow 
{
    Q_OBJECT

public:
    explicit CSynthVisualizationWindow(CAudioConsumer* audioConsumer, CAudioGenerator* audioGenerator, int numHarmonics = AudioConstants::NUM_HARMONICS, QWidget* parent = nullptr);

private:
    void setupUI();
    void connectSignals();
    void startVisualization();
    void updateAudioParameters();
    void updateActiveHarmonics(int count);

    int m_numHarmonics;
    int m_activeHarmonicCount;

    CAudioConsumer* m_audioConsumer;
    CAudioGenerator* m_audioGenerator;    

    CWaveform m_waveformView;
    CSpectrumAnalyzer m_spectrumAnalyzer;
    CHarmonicControlPanel m_harmonicPanel;
    
    QPushButton* m_recordButton {nullptr};
    QLabel* m_recordingStatusLabel {nullptr};
    

public slots:
    // Set the fundamental frequency for all harmonics
    void setFundamental(double frequency);

    // Set the master output amplitude
    void setMasterAmplitude(double amplitude);

    // Set audio effects parameters
    void setEffects(double distortion, double filter, double reverb);

private slots:
    // Toggle audio recording on or off
    void toggleRecording();
    
    // Update all visualization widgets
    void updateVisualizations();

};

