
#include "Timer.h"

Timer* Timer::instance = nullptr;

Timer* Timer::Instance()
{
    if (instance == nullptr)
    {
        instance = new Timer();
    }
    return instance;
}

void Timer::Destroy()
{
    delete instance;
    instance = nullptr;
}

Timer::Timer()
{
    Reset();
    m_timeScale = 1.0f;
    m_deltaTime = std::chrono::duration<float>(0.0f);
}

 Timer::~Timer()
{

}

void Timer::Reset()
{
    m_startTime = std::chrono::system_clock::now();
}

float Timer::DeltaTime()
{
    return m_deltaTime.count();
}

void Timer::TimeScale(float t)
{
    m_timeScale = t;
}

float Timer::TimeScale()
{
    return m_timeScale;
}

void Timer::Tick()
{
    m_deltaTime = std::chrono::system_clock::now() - m_startTime;
}