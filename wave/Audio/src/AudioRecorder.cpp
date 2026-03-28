#include "AudioRecorder.h"
#include <QDateTime>
#include <QDebug>

CAudioRecorder::CAudioRecorder() 
{
}

CAudioRecorder::~CAudioRecorder() 
{
    if (m_isRecording) 
    {
        stopRecording();
    }
}

bool CAudioRecorder::startRecording() 
{
    if (m_isRecording) 
    {
        return false;
    }

    QDir recordingsDir("recordings");
    if (!recordingsDir.exists()) 
    {
        recordingsDir.mkpath(".");
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    QString filename = QString("recording_%1.wav").arg(timestamp);

    m_currentFilePath = recordingsDir.filePath(filename);
    m_file.setFileName(m_currentFilePath);

    if (!m_file.open(QIODevice::WriteOnly)) 
    {
        // qWarning() << "Failed to open file for recording:" << m_currentFilePath;
        return false;
    }

    m_dataSize = 0;
    writeWavHeader();
    m_isRecording = true;

    // qDebug() << "Started recording to:" << m_currentFilePath;
    return true;
}

void CAudioRecorder::stopRecording() 
{
    if (!m_isRecording) 
    {
        return;
    }

    m_isRecording = false;
    updateWavHeader();
    m_file.close();

    // qDebug() << "Stopped recording. Saved to:" << m_currentFilePath << "Size:" << m_dataSize << "bytes";
}

void CAudioRecorder::writeAudioData(const int16_t* data, int numSamples) 
{
    if (!m_isRecording || !m_file.isOpen()) 
    {
        return;
    }

    qint64 bytesToWrite = numSamples * sizeof(int16_t);
    qint64 bytesWritten = m_file.write(reinterpret_cast<const char*>(data), bytesToWrite);
    
    if (bytesWritten > 0) 
    {
        m_dataSize += bytesWritten;
    }
}

void CAudioRecorder::writeWavHeader() 
{

    QDataStream stream(&m_file);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.writeRawData("RIFF", 4);
    stream << (uint32_t) 36; //Placeholder for the total File size
    stream.writeRawData("WAVE", 4);

    stream.writeRawData("fmt ", 4);
    stream << (uint32_t) 16;
    stream << (uint16_t) 1;
    stream << (uint16_t) m_numChannels;
    stream << (uint32_t) m_sampleRate;
    stream << (uint32_t) (m_sampleRate * m_numChannels * m_bitsPerSample / 8);  // Byte rate
    stream << (uint16_t) (m_numChannels * m_bitsPerSample / 8);  // Block align
    stream << (uint16_t) m_bitsPerSample;

    stream.writeRawData("data", 4);
    stream << (uint32_t) 0;  // Placeholder for total Data size
}

void CAudioRecorder::updateWavHeader() 
{
    if (!m_file.isOpen()) 
    {
        return;
    }

    // Update the file size
    m_file.seek(4);
    QDataStream stream(&m_file);

    stream.setByteOrder(QDataStream::LittleEndian);
    stream << (uint32_t)(m_dataSize + 36);  // Total file size - 8

    // Update data chunk size
    m_file.seek(40);
    stream << (uint32_t)m_dataSize;
}
