#include <QAudioSink>
#include <QMediaDevices>
#include <iostream>
#include <cmath>
#include <vector>
#include "AudioGenerator.h"
#include "SynthVisualizationWindow.h"
#include "WaveController.h"
#include "Constants.h"

CWaveController::CWaveController(QObject *parent) : QObject(parent) 
{
    QAudioFormat format;
    format.setSampleRate(AudioConstants::SAMPLE_RATE);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice info = QMediaDevices::defaultAudioOutput();
    
    m_audioGenerator = new CAudioGenerator(this);
    m_audioSink = new QAudioSink(info, format, this);
    m_audioSink->setBufferSize(AudioConstants::BUFFER_SIZE);
    
    m_audioGenerator->start();
    m_audioSink->start(m_audioGenerator);
        
    m_visualizer = new CSynthVisualizationWindow(m_audioGenerator, AudioConstants::NUM_HARMONICS);
    m_visualizer->show();
    
    //Audio after visualizer as GUI takes a while to load!!
    m_audioGenerator->setFundamental(220.0);
    m_audioGenerator->setMasterAmplitude(0.5);

    m_visualizer->setFundamental(220.0);
    m_visualizer->setMasterAmplitude(0.5);
}