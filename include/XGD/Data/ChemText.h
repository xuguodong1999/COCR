#pragma once

#include "xgd_data_export.h"
#include <unordered_set>
#include <functional>
#include <string>
#include <vector>
#include <optional>

class XGD_DATA_EXPORT SpliceableText {
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

    bool isFull() const;

    bool operator==(const SpliceableText &_st) const;

    explicit SpliceableText();

    explicit SpliceableText(std::string _text, int _l, int _r);


    const std::string &getRaw() const;

    std::string getRand() const;

    std::string getNeutral() const;
};

class XGD_DATA_EXPORT LineTextDataCreator {
    std::unordered_set<char> charSet;

    std::unordered_set<std::string> wordSet;
public:
    const std::unordered_set<char> &getCharSet() const;

    const std::unordered_set<std::string> &getWordSet() const;

    void loadFromWordDict(const char *_filepath);

    void loadFromPattern(const std::string &filepath);

    void clearSet();

    void updateCharSet();

    void loopOnce(const std::function<void(const std::string &)> &_func);

    void displaySet();
};