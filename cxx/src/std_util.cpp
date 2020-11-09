#include "std_util.hpp"

using namespace std;

string trim(const string &str) {
    static string blanks("\f\v\r\t\n ");
    string ret = str;
    ret.erase(0, str.find_first_not_of(blanks));
    ret.erase(str.find_last_not_of(blanks) + 1);
    return move(ret);
}