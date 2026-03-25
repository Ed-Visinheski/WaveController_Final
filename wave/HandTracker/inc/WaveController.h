#pragma once
#include <QObject>
#include <vector>
#include "Constants.h"

class QAudioSink;
class CAudioGenerator;
class CSynthVisualizationWindow;

class CWaveController : public QObject 
{
    Q_OBJECT

public:
    CWaveController(QObject *parent = nullptr);

private:
    CAudioGenerator* m_audioGenerator;
    QAudioSink *m_audioSink;
    CSynthVisualizationWindow *m_visualizer;

};