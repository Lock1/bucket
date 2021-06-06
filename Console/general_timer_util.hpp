#ifndef GENERAL_TIMER_UTIL_HPP
#define GENERAL_TIMER_UTIL_HPP

#include <chrono>
#include <thread>

class UtilityTimer {
private:
    std::chrono::system_clock::time_point clock_start;
    std::chrono::system_clock::time_point clock_end;
    std::chrono::system_clock::duration clock_duration;

public:
    UtilityTimer() {

    }

    void startClock() {
        clock_start = std::chrono::system_clock::now();
    }

    void stopClock() {
        clock_end = std::chrono::system_clock::now();
        clock_duration = (clock_end - clock_start);
    }

    double getElapsedNano() {
        return clock_duration.count();
    }

    double getElapsedMicro() {
        return clock_duration.count()/1000;
    }

    double getElapsedMili() {
        return clock_duration.count()/(1000*1000);
    }

    static void sleep_thread_millisecond(unsigned long duration) {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }
};

#endif
