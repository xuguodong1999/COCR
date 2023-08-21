#pragma once

#include "xgd_base_export.h"
#include <chrono>

class XGD_BASE_EXPORT Timer {
    decltype(std::chrono::system_clock::now()) start_stamp, end_stamp, last_stamp;
public:
    void start(bool _display = false);

    void display_duration();

    void stop(bool _display_duration = true);
};
