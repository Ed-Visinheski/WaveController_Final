#pragma once

#include <QObject>
#include <QTimer>

/**
 * CTimer: Centralized singleton timer for synchronised visualization updates
 * Provides a 60 FPS timer shared across components
 */
class CTimer : public QObject 
{
    Q_OBJECT

public:
    static constexpr int UPDATE_FPS = 60;
    static constexpr int UPDATE_INTERVAL_MILLISECONDS = 1000 / UPDATE_FPS;

    // Get the timer instance
    static CTimer& instance();

    // Start the timer
    void start();

    // Stop the timer
    void stop();

    // Check if the timer is currently running
    bool isRunning() const;
    
private:
    CTimer();
    ~CTimer() override = default;

    CTimer(CTimer&&) = delete;
    CTimer& operator=(CTimer&&) = delete;

    CTimer(const CTimer&) = delete;
    CTimer& operator=(const CTimer&) = delete;

    QTimer* m_timer;

signals:
    void timeout();

};
