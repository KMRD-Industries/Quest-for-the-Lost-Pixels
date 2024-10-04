#pragma once

#include <chrono>

class Timer {

private:
    static Timer* instance;
    std::chrono::system_clock::time_point m_startTime;
    std::chrono::duration<float> m_deltaTime;
    float m_timeScale;

public:
    static Timer* Instance();
    static void Destroy();
    void Reset();
    float DeltaTime();
    void TimeScale(float t = 1.0f);
    float TimeScale();
    void Tick();

private:
    Timer();
    ~Timer();
};
