#include "config.hpp"
#include <filesystem>

using namespace std;
#ifdef WIN32
string WORKSPACE("C:/source/repos/jokejoker/workspace");
#else
string WORKSPACE("/home/xgd/source/repos/jokejoker/workspace/");
#endif

string COUCH_DIR(WORKSPACE + "/couch");

string COUCH_SYM_PATH(COUCH_DIR + "/couch-sym.dat");

string WEIGHTS_DIR(WORKSPACE + "/weights");

string ALKANE_DIR("C:/Users/xgd/source/COCR/data/alkane");

string DRUGBANK_PATH("C:/Users/xgd/source/COCR/data/drugbank/full_database.xml");

unordered_map<string, string &> COCR_DIRS;
unordered_map<string, string &> COCR_PATHS;
