#pragma once

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <vector>
#include <cstdint>
#include "Constants.h"

/**
 * CAudioRecorder: Records audio samples to WAV file format
 * Handles the WAV file creation, header WAV format writing, and timestamped file naming
 */
class CAudioRecorder 
{
public:
    CAudioRecorder();
    ~CAudioRecorder();

    // Start recording to a WAV file with auto generated filename
    bool startRecording();

    // Stop recording and update the WAV file
    void stopRecording();

    // Check if currently recording
    bool isRecording() const { return m_isRecording; }
    
    // Write audio samples to the recording file
    void writeAudioData(const int16_t* data, int numSamples);
    
    // Get the path of the current recording
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
