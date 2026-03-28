#pragma once

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <vector>
#include <cstdint>
#include "Constants.h"

class CAudioRecorder 
{
public:
    CAudioRecorder();
    ~CAudioRecorder();

    bool startRecording(const QString& filename);
    void stopRecording();
    bool isRecording() const { return m_isRecording; }
    
    void writeAudioData(const int16_t* data, int numSamples);
    
    QString getLastRecordingPath() const { return m_currentFilePath; }

private:
    void writeWavHeader();
    void updateWavHeader();
    
    QFile m_file;
    QString m_currentFilePath;
    bool m_isRecording{false};
    
    int m_sampleRate{AudioConstants::SAMPLE_RATE};
    int m_numChannels{1};
    int m_bitsPerSample{16};
    uint32_t m_dataSize{0};
};
