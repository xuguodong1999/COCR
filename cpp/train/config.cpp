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

