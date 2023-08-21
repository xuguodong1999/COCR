#include "XGD/Base/Timer.h"

#include "XGD/SpdlogStream/Stream.h"

#include <iostream>

using namespace std;
using namespace chrono;

void Timer::start(bool _display) {
    start_stamp = last_stamp = system_clock::now();
    if (_display) {
        xgd::sout << "Timer start" << std::endl;
    }
}

void Timer::display_duration() {
    end_stamp = system_clock::now();
    auto duration = duration_cast<microseconds>(
            end_stamp - last_stamp);
    xgd::sout << "Timer mid: [takes "
              << duration.count() * milliseconds::period::num / milliseconds::period::den
              << " ms]" << ::endl;
    last_stamp = end_stamp;
}

void Timer::stop(bool _display_duration) {
    end_stamp = last_stamp = system_clock::now();
    if (_display_duration) {
        auto duration = duration_cast<microseconds>(
                end_stamp - start_stamp);
        xgd::sout << "Timer end: [takes "
                  << duration.count() * milliseconds::period::num / milliseconds::period::den
                  << " ms]" << ::endl;
    }
}