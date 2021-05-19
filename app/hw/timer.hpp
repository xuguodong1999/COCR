/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <chrono>

class Timer {
    decltype(std::chrono::system_clock::now())
            start_stamp, end_stamp, last_stamp;
public:
    void start(bool _display = false);

    float display_duration();

    void stop(bool _display_duration = true);
};

#endif//_TIMER_HPP_
