//
// Created by Piotr on 13.12.2024.
//

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    double time_limit;  // Limit czasu
    double elapsed_time;

    // Konstruktor, który ustawia limit czasu
    explicit Timer(double limit);

    // Rozpoczęcie pomiaru czasu
    void startCounter();

    // Pobranie czasu w milisekundach od rozpoczęcia pomiaru
    double getCounter();

private:

    std::chrono::system_clock::time_point start; // Czas rozpoczęcia
    std::chrono::high_resolution_clock::time_point end; // Czas zakończenia
};



#endif //TIMER_H
