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
    struct SHarmonicControl 
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
    void setActiveHarmonicCount(int count);
    int getActiveHarmonicCount() const { return m_activeHarmonicCount; }

private:
    void setupUI();
    void updateFrequencyLabels();
    void updateSoloState();
    void updateVisibleControls();

    bool m_soloActive;
    int m_numHarmonics;
    int m_activeHarmonicCount;
    double m_fundamental = 440.0;

    std::vector<SHarmonicControl> m_controls;
    std::vector<QWidget*> m_controlWidgets;

signals:
    void parametersChanged();
    void harmonicChanged(int harmonicNumber);
    void harmonicCountChanged(int count);
};

