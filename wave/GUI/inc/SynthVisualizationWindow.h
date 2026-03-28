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

class CSynthVisualizationWindow : public QMainWindow 
{
    Q_OBJECT

public:
    explicit CSynthVisualizationWindow(CAudioGenerator* audioGen, int numHarmonics = AudioConstants::NUM_HARMONICS, QWidget* parent = nullptr);

private:
    void setupUI();
    void connectSignals();
    void startVisualization();
    void updateAudioParameters();
    void updateActiveHarmonics(int count);

    int m_numHarmonics;
    int m_activeHarmonicCount;
    QTimer* m_updateTimer {nullptr};

    CAudioGenerator* m_audioGenerator;    
    CWaveform* m_waveformView {nullptr};
    CSpectrumAnalyzer* m_spectrumAnalyzer = nullptr;
    CHarmonicControlPanel* m_harmonicPanel {nullptr};
    
    QPushButton* m_recordButton {nullptr};
    QLabel* m_recordingStatusLabel {nullptr};
    

public slots:
    void setFundamental(double frequency);
    void setMasterAmplitude(double amplitude);
    void setEffects(double distortion, double filter, double reverb);

private slots:
    void toggleRecording();
    void updateVisualizations();
};

