#include "config.hpp"
#include <filesystem>

using namespace std;
#ifdef WIN32
string WORKSPACE("C:/Users/xgd/source/repos/leafxy/workspace/");
string META_DATA_DIR("C:/Users/xgd/source/repos/leafxy/resources/");
#else
string WORKSPACE("/home/xgd/source/repos/leafxy/workspace/");
string META_DATA_DIR("/home/xgd/source/repos/leafxy/resources/");
#endif

string COUCH_DIR(META_DATA_DIR + "couch/");

string COUCH_SYM_PATH(COUCH_DIR + "couch-sym.dat");

string WORD_DICT_PATH(META_DATA_DIR + "words_dictionary.json");

string WEIGHTS_DIR(WORKSPACE + "weights/");

