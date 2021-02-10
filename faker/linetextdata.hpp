#ifndef _LINE_TEXT_DATA_HPP_
#define _LINE_TEXT_DATA_HPP_

#include <unordered_set>
#include <functional>
#include <string>

class LineTextDataCreator {
    std::unordered_set<char> charSet;
    std::unordered_set<std::string> wordSet;
public:
    void loadFromSuperAtom(const char *_filepath =
    ":/obdata/superatom.txt");

    void loadFromWordDict(const char *_filepath =
    "/home/xgd/Downloads/words_dictionary.json");

    void loadFromPattern(const char *_filepath =
    ":/obdata/superatom.txt");

    void clearSet();

    void updateCharSet();

    void loopOnce(const std::function<void(const std::string &)> &_func);
    void displaySet();
};

#endif//_LINE_TEXT_DATA_HPP_
