#include "config.hpp"
#include <filesystem>

using namespace std;
#ifdef WIN32
string WORKSPACE("C:/Users/xgd/source/repos/jokejoker/workspace/");
#else
string WORKSPACE("/home/xgd/source/repos/jokejoker/workspace/");
#endif

string COUCH_DIR(WORKSPACE + "couch/");

string COUCH_SYM_PATH(COUCH_DIR + "couch-sym.dat");

string WEIGHTS_DIR(WORKSPACE + "weights/");

string WORD_DICT_PATH(WORKSPACE + "words_dictionary.json");

