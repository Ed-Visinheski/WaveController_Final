#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QStatusBar>
#include <QTimer>
#include <QDebug>
#include <memory>
#include "WaveForm.h"
#include "HarmonicControlPanel.h"
#include "AudioGenerator.h"
#include "Constants.h"

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

    int m_numHarmonics;
    QTimer* m_updateTimer {nullptr};

    CAudioGenerator* m_audioGenerator;    
    CWaveform* m_waveformView {nullptr};
    CHarmonicControlPanel* m_harmonicPanel {nullptr};
    

public slots:
    void setFundamental(double frequency);
    void setMasterAmplitude(double amplitude);
    void setEffects(double distortion, double filter, double reverb);

private slots:
    void updateVisualizations();
};

