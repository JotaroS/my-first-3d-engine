#include <chrono>
#include<iostream>
#include <mach/mach_time.h>
#include "functions.hpp"
#include<iomanip>

template<typename T>
struct Interpolated
{
    T start{};
    T end{};
    float speed{1.0f}; // units per second
    float start_time{};
    TransitionFunction func{TransitionFunction::EaseOutCubic};
    
    explicit
    Interpolated(const T& initial_value)
        : start(initial_value), end(initial_value), start_time(getCurrentTime()), speed(1.0f)
    {
    }

    [[nodiscard]]
    static float getCurrentTime()
    {
        // using namespace std::chrono;
        // auto now = std::chrono::high_resolution_clock::now();
        // auto const duration = now.time_since_epoch();
        // auto const seconds = std::chrono::duration_cast<std::chrono::duration<float> >(duration);
        // std::cout<<std::fixed << std::setprecision(6)<< seconds.count()<<std::endl;
        // return seconds.count();
        
        // get mach time in float seconds

        static mach_timebase_info_data_t timebase;
        if (timebase.denom == 0) {
            mach_timebase_info(&timebase);
        }
        return (float)mach_absolute_time() * timebase.numer / timebase.denom / 1e9;
    }

    [[nodiscard]]

    static float getElapsedTime(float start_time)
    {
        return getCurrentTime() - start_time;
    }

    [[nodiscard]]

    T getValue() const
    {
        float elapsed = getElapsedTime(start_time);
        float t = elapsed / (float)speed; // using speed for dynamic duration
        if (t >= 1.0f)
            return end;

        if (config::interpolationOverride == false){
            return end;
        }
        
        return start + (end - start) * getRatio(t, func);
    }

    
    [[nodiscard]]
    operator T() const
    {
        return getValue();
    }

    // convenience accessors so callers can read components
    [[nodiscard]] T get() const { return getValue(); }
    [[nodiscard]] T value() const { return getValue(); }
    [[nodiscard]] T operator*() const { return getValue(); }
    [[nodiscard]] T operator->() const { return getValue(); }


    [[nodiscard]]
    Interpolated& operator = (const T& new_value)
    {
        start = getValue();
        end = new_value;
        start_time = getCurrentTime();
        return *this;
    }

    void setDuration(float duration)
    {
        if (duration <= 0.0f)
            return;
        speed = duration;
    }

    void forceSet(const T& new_value)
    {
        start = new_value;
        end = new_value;
        start_time = getCurrentTime();
    }

};