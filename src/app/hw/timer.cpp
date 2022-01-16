/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#include "timer.hpp"
#include <iostream>

using namespace std;
using namespace chrono;

void Timer::start(bool _display) {
    start_stamp = last_stamp = system_clock::now();
    if (_display) {
        cout << "Timer start" << std::endl;
    }
}

float Timer::display_duration() {
    end_stamp = system_clock::now();
    auto duration = duration_cast<microseconds>(
            end_stamp - last_stamp);
    float delta = duration.count() * milliseconds::period::num / milliseconds::period::den;
    cout << "Timer mid: [takes "
         << delta
         << " ms]" << ::endl;
    last_stamp = end_stamp;
    return delta;
}

void Timer::stop(bool _display_duration) {
    end_stamp = last_stamp = system_clock::now();
    if (_display_duration) {
        auto duration = duration_cast<microseconds>(
                end_stamp - start_stamp);
        cout << "Timer end: [takes "
             << duration.count() * milliseconds::period::num / milliseconds::period::den
             << " ms]" << ::endl;
    }
}