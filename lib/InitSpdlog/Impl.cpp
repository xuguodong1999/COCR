#include "XGD/Init/Spdlog.h"

#include <spdlog/spdlog.h>

#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#endif

namespace xgd::init {
    bool setupSpdlog() {
#ifdef __ANDROID__
        auto default_logger = spdlog::android_logger_mt("spdlog");
        spdlog::set_default_logger(default_logger);
        spdlog::set_pattern("%v");
#else
        spdlog::set_pattern("[%Y-%m-%d %T.%e] [%l] [thread %-6t] %v");
#endif
        spdlog::info("spdlog init done.");
        return true;
    }
}

//static const bool SPDLOG_INIT = []() {
//    setupSpdlog()
//    return true;
//}();
