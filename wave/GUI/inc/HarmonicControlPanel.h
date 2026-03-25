#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <vector>
#include "Constants.h"

class CHarmonicControlPanel : public QWidget 
{
    Q_OBJECT

public:
    struct HarmonicControl 
    {
        int harmonicNumber;
        
        QLabel* frequencyLabel;
        QLabel* amplitudeLabel;

        QCheckBox* muteCheckbox;
        QPushButton* soloButton;

        QSlider* amplitudeSlider;
        QSlider* phaseSlider;
    };

    explicit CHarmonicControlPanel(int numHarmonics = AudioConstants::NUM_HARMONICS, QWidget* parent = nullptr);

    void setFundamental(double frequency);

    double getHarmonicPhase(int harmonicNumber) const;
    double getHarmonicAmplitude(int harmonicNumber) const;

    void resetToDefault();
    void setProfile(const QString& profileName);

private:
    void setupUI();
    void updateFrequencyLabels();
    void updateSoloState();

    bool m_soloActive;
    int m_numHarmonics;
    double m_fundamental = 440.0;

    std::vector<HarmonicControl> m_controls;

signals:
    void parametersChanged();
    void harmonicChanged(int harmonicNumber);
};

