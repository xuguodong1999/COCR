#include "timer.hpp"
#include <iostream>

using namespace std;
using namespace chrono;

void Timer::start() {
    start_stamp = last_stamp = system_clock::now();
    cout << "Timer start" << std::endl;
}

void Timer::display_duration() {
    end_stamp = system_clock::now();
    auto duration = duration_cast<microseconds>(
            end_stamp - last_stamp);
    cout << "Timer mid: [takes "
         << duration.count() * milliseconds::period::num / milliseconds::period::den
         << " ms]" << ::endl;
    last_stamp = end_stamp;
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