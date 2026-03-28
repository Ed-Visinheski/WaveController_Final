#include "Timer.h"

CTimer::CTimer() : QObject(nullptr), m_timer(new QTimer(this))
{
    m_timer->setInterval(UPDATE_INTERVAL_MILLISECONDS);
    m_timer->setTimerType(Qt::PreciseTimer);
    
    connect(m_timer, &QTimer::timeout, this, &CTimer::timeout);
}

CTimer& CTimer::instance() 
{
    static CTimer instance;
    return instance;
}

void CTimer::start() 
{
    if (!m_timer->isActive()) 
    {
        m_timer->start();
    }
}

void CTimer::stop() 
{
    m_timer->stop();
}

bool CTimer::isRunning() const 
{
    return m_timer->isActive();
}
