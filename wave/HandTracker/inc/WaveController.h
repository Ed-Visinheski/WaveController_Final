#pragma once
#include <QObject>
#include <vector>
#include "Constants.h"

class QUdpSocket;
class QAudioSink;

class CAudioGenerator;
class CSynthVisualizationWindow;

class CWaveController : public QObject 
{
    Q_OBJECT

public:
    CWaveController(QObject *parent = nullptr);

private:
    QUdpSocket* m_udpSocket;
    CAudioGenerator* m_audioGenerator;
    QAudioSink *m_audioSink;
    CSynthVisualizationWindow *m_visualizer;

    const std::vector<double> PENTATONIC_RATIOS = {1.0, 9.0/8.0, 5.0/4.0, 3.0/2.0, 5.0/3.0, 2.0};

private slots:
    void processPendingDatagrams();
};