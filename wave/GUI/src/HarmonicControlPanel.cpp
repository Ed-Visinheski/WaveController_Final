#include "HarmonicControlPanel.h"

CHarmonicControlPanel::CHarmonicControlPanel(int numHarmonics, QWidget* parent)
                                            : QWidget(parent)
                                            , m_numHarmonics(numHarmonics)
                                            , m_soloActive(false)
{
    setupUI();
}

void CHarmonicControlPanel::setFundamental(double frequency) 
{
    m_fundamental = frequency;
    updateFrequencyLabels();
}

double CHarmonicControlPanel::getHarmonicAmplitude(int harmonicNumber) const 
{
    if (harmonicNumber < 1 || harmonicNumber > m_numHarmonics) 
    {
        return 0.0;
    }
    
    const auto& control = m_controls[harmonicNumber - 1];
    
    if (control.muteCheckbox->isChecked())
    {
        return 0.0;
    } 
    
    if (m_soloActive && !control.soloButton->isChecked()) 
    {
        return 0.0;
    }
    return control.amplitudeSlider->value() / 100.0;
}

double CHarmonicControlPanel::getHarmonicPhase(int harmonicNumber) const 
{
    if (harmonicNumber < 1 || harmonicNumber > m_numHarmonics)
    {
        return 0.0;
    }
    
    const auto& control = m_controls[harmonicNumber - 1];
    return (control.phaseSlider->value() / 100.0) * 2.0 * M_PI;
}

void CHarmonicControlPanel::resetToDefault() 
{
    for (auto& control : m_controls) 
    {
        control.amplitudeSlider->setValue(100 / m_numHarmonics);
        control.phaseSlider->setValue(0);
        control.muteCheckbox->setChecked(false);
        control.soloButton->setChecked(false);
    }
    m_soloActive = false;
}

void CHarmonicControlPanel::setProfile(const QString& profileName) 
{
    if 
    (profileName == "Sawtooth") 
    {
        for (int i = 0; i < m_numHarmonics; ++i) 
        {
            m_controls[i].amplitudeSlider->setValue(100 / (i + 1));
        }
    } 
    else if (profileName == "Square") 
    {
        for (int i = 0; i < m_numHarmonics; ++i) 
        {
            if ((i + 1) % 2 == 1) 
            {
                m_controls[i].amplitudeSlider->setValue(100 / (i + 1));
            } 
            else 
            {
                m_controls[i].amplitudeSlider->setValue(0);
            }
        }
    } 
    else if (profileName == "Triangle") 
    {
        for (int i = 0; i < m_numHarmonics; ++i) 
        {
            if ((i + 1) % 2 == 1) 
            {
                const int amplitude = 100 / ((i + 1) * (i + 1));
                m_controls[i].amplitudeSlider->setValue(amplitude);
            } 
            else 
            {
                m_controls[i].amplitudeSlider->setValue(0);
            }
        }
    } 
    else if (profileName == "Sine") 
    {
        for (int i = 0; i < m_numHarmonics; ++i) 
        {
            m_controls[i].amplitudeSlider->setValue(i == 0 ? 100 : 0);
        }
    }
}

void CHarmonicControlPanel::setupUI() 
{
    auto* mainLayout = new QVBoxLayout(this);
    
    auto* presetBox = new QGroupBox("Waveform Presets");
    auto* presetLayout = new QHBoxLayout(presetBox);
    
    const QStringList presets = {"Sine", "Sawtooth", "Square", "Triangle", "Reset"};

    for (const QString& preset : presets) 
    {
        auto* btn = new QPushButton(preset);
        connect(btn, &QPushButton::clicked, this, [this, preset]() 
        {
            if (preset == "Reset") 
            {
                resetToDefault();
            } 
            else 
            {
                setProfile(preset);
            }
            emit parametersChanged();
        });

        presetLayout->addWidget(btn);
    }

    mainLayout->addWidget(presetBox);

    auto* controlsBox = new QGroupBox("Harmonic Controls");
    auto* controlsLayout = new QVBoxLayout(controlsBox);
    
    for (int i = 0; i < m_numHarmonics; ++i) 
    {
        SHarmonicControl control;
        control.harmonicNumber = i + 1;
        
        auto* rowWidget = new QWidget();
        auto* rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        
        auto* harmLabel = new QLabel(QString("H%1:").arg(i + 1));
        harmLabel->setMinimumWidth(30);
        rowLayout->addWidget(harmLabel);
        
        control.frequencyLabel = new QLabel("440 Hz");
        control.frequencyLabel->setMinimumWidth(70);
        rowLayout->addWidget(control.frequencyLabel);
        
        control.amplitudeSlider = new QSlider(Qt::Horizontal);
        control.amplitudeSlider->setRange(0, 100);
        control.amplitudeSlider->setValue(100 / m_numHarmonics);
        control.amplitudeSlider->setMinimumWidth(150);
        rowLayout->addWidget(control.amplitudeSlider);
        
        control.amplitudeLabel = new QLabel("100%");
        control.amplitudeLabel->setMinimumWidth(40);
        rowLayout->addWidget(control.amplitudeLabel);
        
        auto* phaseLabel = new QLabel("Phase:");
        rowLayout->addWidget(phaseLabel);
        control.phaseSlider = new QSlider(Qt::Horizontal);
        control.phaseSlider->setRange(0, 100);
        control.phaseSlider->setValue(0);
        control.phaseSlider->setMinimumWidth(80);
        rowLayout->addWidget(control.phaseSlider);
        
        control.muteCheckbox = new QCheckBox("Mute");
        rowLayout->addWidget(control.muteCheckbox);
        
        control.soloButton = new QPushButton("Solo");
        control.soloButton->setCheckable(true);
        control.soloButton->setMaximumWidth(200);
        rowLayout->addWidget(control.soloButton);
        
        connect(control.amplitudeSlider, &QSlider::valueChanged, this, [this, i](int value) 
        {
            m_controls[i].amplitudeLabel->setText(QString("%1%").arg(value));
            emit harmonicChanged(i + 1);
            emit parametersChanged();
        });
        
        connect(control.phaseSlider, &QSlider::valueChanged, this, [this, i]() 
        {
            emit harmonicChanged(i + 1);
            emit parametersChanged();
        });
        
        connect(control.muteCheckbox, &QCheckBox::toggled, this, [this, i]() 
        {
            emit harmonicChanged(i + 1);
            emit parametersChanged();
        });
        
        connect(control.soloButton, &QPushButton::toggled, this, [this, i](bool checked) 
        {
            updateSoloState();
            emit harmonicChanged(i + 1);
            emit parametersChanged();
        });
        
        controlsLayout->addWidget(rowWidget);
        m_controls.push_back(control);
    }
    
    mainLayout->addWidget(controlsBox);
    mainLayout->addStretch();
}

void CHarmonicControlPanel::updateFrequencyLabels() 
{
    for (size_t i {0}; i < m_controls.size(); ++i) 
    {
        const double freq = m_fundamental * (i + 1);
        m_controls[i].frequencyLabel->setText(QString("%1 Hz").arg(freq, 0, 'f', 1));
    }
}

void CHarmonicControlPanel::updateSoloState() 
{
    m_soloActive = false;
    for (const auto& control : m_controls) 
    {
        if (control.soloButton->isChecked()) 
        {
            m_soloActive = true;
            break;
        }
    }
}


