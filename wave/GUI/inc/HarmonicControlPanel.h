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

/**
 * CHarmonicControlPanel UI panel for controlling individual harmonic amplitudes and phases
 * Provides sliders, with mute and solo controls, and preset profiles for harmonic generation
 */
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

    // Set the fundamental frequency and updates all harmonic labels
    void setFundamental(double frequency);

    // Get the phase offset for a specific harmonic

    double getHarmonicPhase(int harmonicNumber) const;

    // Get the amplitude for a specific harmonic
    double getHarmonicAmplitude(int harmonicNumber) const;

    // Reset all harmonics to default values
    void resetToDefault();

    // Load the premade harmonic profiles like Sine, Saw and square
    void setProfile(const QString& profileName);

    // Set the number of active harmonics frpom 1 to 8
    void setActiveHarmonicCount(int count);

    // Get the current number of active harmonics
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

