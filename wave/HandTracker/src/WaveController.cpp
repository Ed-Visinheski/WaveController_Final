#include <QUdpSocket>
#include <QHostAddress>
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
    m_udpSocket = new QUdpSocket(this);

    if (!m_udpSocket->bind(QHostAddress::LocalHost, 5005)) 
    {
        std::cerr << "Failed to bind UDP socket to port 5005" << std::endl;
    }

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &CWaveController::processPendingDatagrams);

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

void CWaveController::processPendingDatagrams() 
{
    while (m_udpSocket->hasPendingDatagrams()) 
    {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());

        QString data = QString::fromUtf8(datagram);
        QStringList parts = data.split(',');
        
        if (parts.size() >= 3) 
        {
            int x = parts[0].toInt();
            int y = parts[1].toInt();
            int isPinch = parts[2].toInt();

            double normalizedY = 1.0 - (static_cast<double>(y) / 800.0);
            normalizedY = std::max(0.0, std::min(1.0, normalizedY));
            
            int totalNotes = PENTATONIC_RATIOS.size() * 3;
            int noteIndex = static_cast<int>(normalizedY * totalNotes);
            noteIndex = std::max(0, std::min(totalNotes - 1, noteIndex));
            
            int octave = noteIndex / PENTATONIC_RATIOS.size();
            int scaleIndex = noteIndex % PENTATONIC_RATIOS.size();
            
            double baseFreq = 110.0;
            double freq = baseFreq * std::pow(2.0, octave) * PENTATONIC_RATIOS[scaleIndex];
            
            double distortion = isPinch ? 0.6 : 0.0;
            double reverb = isPinch ? 0.4 : 0.15;
            double filter = 0.3 + normalizedY * 0.7;
            double amplitude = 0.7;
            
            m_audioGenerator->setFundamental(freq);
            m_audioGenerator->setMasterAmplitude(amplitude);
            m_audioGenerator->setDistortion(distortion);
            m_audioGenerator->setFilter(filter);
            m_audioGenerator->setReverbMix(reverb);
            
            m_visualizer->setFundamental(freq);
            m_visualizer->setMasterAmplitude(amplitude);
            m_visualizer->setEffects(distortion, filter, reverb);
        }
    }
}