#ifndef _LINE_TEXT_DATA_HPP_
#define _LINE_TEXT_DATA_HPP_

#include <unordered_set>
#include <functional>
#include <string>

class SpliceableText {
    int l, r;
    std::string text;
    inline static std::vector<std::string> cons = {"", "-", "=", "#"};
public:
    friend SpliceableText operator+(const SpliceableText &_st, int i);

    friend SpliceableText operator+(int i, const SpliceableText &_st);

    friend SpliceableText operator+(
            const SpliceableText &_st1, const SpliceableText &_st2);

    friend std::optional<SpliceableText> joinSpliceableText(
            const SpliceableText &_st1, const SpliceableText &_st2);
    bool isFull()const;

    bool operator==(const SpliceableText & _st) const;
    explicit SpliceableText();

    explicit SpliceableText(std::string _text, int _l, int _r);


    const std::string &getRaw() const;

    std::string getRand() const;

    std::string getNeutral() const;
};

struct HashSpliceableText {
    size_t operator()(const SpliceableText &_st) const {
        return std::hash<std::string>()(_st.getRaw());
    }
};

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
