#include "AudioConsumer.h"


CAudioConsumer::CAudioConsumer(QObject* parent) : QIODevice(parent)
{
}

void CAudioConsumer::start() 
{
    open(QIODevice::ReadOnly);
}

qint64 CAudioConsumer::bytesAvailable() const 
{
    return AudioConstants::BUFFER_SIZE * 2 + QIODevice::bytesAvailable();
}

qint64 CAudioConsumer::readData(char* data, qint64 len)
{
    emit requestGuiUpdate();

    qint64 total {0};
    int16_t* qtAudioSampleBuffer = reinterpret_cast<int16_t*>(data);
    
    size_t numOfSamples = ((len / 2) < m_audioOutputBuffer->getAvailableRead()) ? (len / 2) : m_audioOutputBuffer->getAvailableRead();
    
    // Read audio data for playback from int16_t buffer
    total = m_audioOutputBuffer->read(qtAudioSampleBuffer, numOfSamples);

    // Record if active
    if(m_recorder.isRecording())
    {
        m_recorder.writeAudioData(qtAudioSampleBuffer, total);
    }

    emit generateAudio();

    return total << 1;
}

bool CAudioConsumer::startRecording() 
{
    return m_recorder.startRecording();
}

void CAudioConsumer::stopRecording() 
{
    m_recorder.stopRecording();
}

bool CAudioConsumer::isRecording() const 
{
    return m_recorder.isRecording();
}

QString CAudioConsumer::getLastRecordingPath() const 
{
    return m_recorder.getLastRecordingPath();
}

qint64 CAudioConsumer::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

