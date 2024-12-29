#include "Timer.h"



Timer::Timer(double limit) : time_limit(limit) {}


void Timer::startCounter() {
    start = std::chrono::high_resolution_clock::now();
}

double Timer::getCounter() {
    end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count();
}