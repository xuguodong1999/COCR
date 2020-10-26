#include "config.hpp"

const char *get_couch_data_path() {
#ifdef WIN32
    return "C:\\Users\\xgd\\source\\COCR\\data\\couch.dat";
#elif defined(unix)
    return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/couch.dat";
#endif
}

const char *get_coordinates_json_path(int id) {
    switch (id) {
        case 2: {
#ifdef WIN32
            return "C:\\Users\\xgd\\source\\COCR\\data\\drugbank.json";
#elif defined(unix)
            return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/drugbank.json";
#endif
        }
        case 3: {
#ifdef WIN32
            return "C:\\Users\\xgd\\source\\COCR\\data\\drugbank_h.json";
#elif defined(unix)
            return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/drugbank_h.json";
#endif
        }
        default: {
#ifdef WIN32
            return "C:\\Users\\xgd\\source\\COCR\\data\\fake_chonps12345.json";
#elif defined(unix)
            return "/media/xgd/Windows-SSD/Users/xgd/source/COCR/data/fake_chonps12345.json";
#endif
        }
    }
}


void Timer::start() {
    start_stamp = last_stamp = std::chrono::system_clock::now();
}

void Timer::display_duration() {
    end_stamp = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_stamp - last_stamp);
    std::cout << "Timer mid: [takes "
              << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
              << " ms]" << std::endl;
    last_stamp = end_stamp;
}

void Timer::stop(bool _display_duration) {
    end_stamp = last_stamp = std::chrono::system_clock::now();
    if (_display_duration) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                end_stamp - start_stamp);
        std::cout << "Timer end: [takes "
                  << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
                  << " ms]" << std::endl;
    }
}

std::string trim(const string &str) {
    static std::string blanks("\f\v\r\t\n ");
    std::string ret = str;
    ret.erase(0, str.find_first_not_of(blanks));
    ret.erase(str.find_last_not_of(blanks) + 1);
    return std::move(ret);
}