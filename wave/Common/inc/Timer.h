#pragma once

#include <QObject>
#include <QTimer>

class CTimer : public QObject 
{
    Q_OBJECT

public:
    static constexpr int UPDATE_FPS = 60;
    static constexpr int UPDATE_INTERVAL_MILLISECONDS = 1000 / UPDATE_FPS;

    static CTimer& instance();

    void start();
    void stop();

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
