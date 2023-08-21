#include "XGD/Data/ChemText.h"
#include "XGD/Base/Util.h"

#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>

#include <iosfwd>
#include <vector>
#include <optional>


struct HashSpliceableText {
    size_t operator()(const SpliceableText &_st) const {
        return std::hash<std::string>()(_st.getRaw());
    }
};

SpliceableText::SpliceableText() : l(0), r(0) {}

SpliceableText::SpliceableText(std::string _text, int _l, int _r)
        : text(std::move(_text)), l(_l), r(_r) {}


const std::string &SpliceableText::getRaw() const {
    return text;
}

std::string SpliceableText::getRand() const {
    if (l && r && StdUtil::byProb(0.6)) {
        return cons[StdUtil::randInt() % std::min(l, 3)] + text + cons[StdUtil::randInt() % std::min(r, 3)];
    } else if (l && StdUtil::byProb(0.6)) {
        return cons[StdUtil::randInt() % std::min(l, 3)] + text;
    } else if (r && StdUtil::byProb(0.6)) {
        // 右侧有自由基
        return text + cons[StdUtil::randInt() % std::min(r, 3)];
    } else {
        return text;
    }
}

std::string SpliceableText::getNeutral() const {
    if (l && r) {
        // 两侧都有自由基
        return cons[std::min(l, 3)] + text + cons[std::min(r, 3)];
    } else if (l) {
        // 右侧有自由基
        return cons[std::min(l, 3)] + text;
    } else if (r) {
        return text + cons[std::min(r, 3)];
    } else {
        return text;
    }
}


std::optional<SpliceableText> joinSpliceableText(
        const SpliceableText &_st1, const SpliceableText &_st2) {
    // -right left-
    // =right left=
    // -leftRight-

    // -thirdGroup=
    // =thirdGroup-
    // #thirdGroup
    // thirdGroup#

    // -fourthGroup#
    // #fourthGroup-
    // =fourthGroup=
    if (_st1.r && _st1.r == _st2.l) {
        return _st1 + _st1.r + _st2;
    } else return std::nullopt;
}

/**
 * add bond. check condition before call it
 * @param i =1,2,3
 * @return
 */

SpliceableText operator+(const SpliceableText &_st, int i) {
    if (i < 0 || i > 3) {
        xgd::serr << "i=" << i << std::endl;
        exit(-1);
    }
    auto st = _st;
    st.r = i;
    if (i == 1 && StdUtil::byProb(0.5)) {
        // ignore -
    } else {
        st.text = st.text + SpliceableText::cons[i];
    }
    return st;
}

SpliceableText operator+(int i, const SpliceableText &_st) {
    if (i < 0 || i > 3) {
        xgd::serr << "i=" << i << std::endl;
        exit(-1);
    }
    auto st = _st;
    st.l = i;
    if (i == 1 && StdUtil::byProb(0.5)) {
        // ignore -
    } else {
        st.text = SpliceableText::cons[i] + st.text;
    }
    return st;
}

// dont call
SpliceableText operator+(const SpliceableText &_st1, const SpliceableText &_st2) {
    SpliceableText st;
    st.l = _st1.l;
    st.r = _st2.r;
    st.text = _st1.text + _st2.text;
    return st;
}

bool SpliceableText::operator==(const SpliceableText &_st) const {
    return text == _st.text && r == _st.r && l == _st.l;
}

bool SpliceableText::isFull() const {
    return !l && !r;
}

void LineTextDataCreator::loadFromWordDict(const char *_filepath) {
    QFile file(_filepath);

    file.open(QIODevice::ReadOnly);

    auto str = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc(QJsonDocument::fromJson(str, &err));

    if (err.error != QJsonParseError::NoError)
        throw std::runtime_error(
                "json:" + err.errorString().toStdString());

    for (auto &key: doc.object().keys()) {
        auto text = key.simplified();
        wordSet.insert(text.toLower().toStdString());
        wordSet.insert(text.toUpper().toStdString());
        if (text.length() > 1) {
            wordSet.insert((text[0].toUpper() + text.mid(1).toLower()).toStdString());
        }

    }
    updateCharSet();
}

void LineTextDataCreator::clearSet() {
    charSet.clear();
    wordSet.clear();
}

void LineTextDataCreator::loopOnce(const std::function<void(const std::string &)> &_func) {
    for (auto &word: wordSet) {
        _func(word);
    }
}

void LineTextDataCreator::displaySet() {
    xgd::sout << "charSet.size=" << charSet.size() << std::endl;
    for (auto &c: charSet) {
        xgd::sout << c << ", ";
    }
    xgd::sout << std::endl;
    xgd::sout << "wordSet.size=" << wordSet.size() << std::endl;
//    for (auto &word:wordSet) {
//        xgd::sout << word << ", ";
//    }
//    xgd::sout << std::endl;
}

void LineTextDataCreator::updateCharSet() {
    charSet.clear();
    for (auto &word: wordSet) {
        for (auto &c: word) {
            charSet.insert(c);
        }
    }
}


// 规则限定、连接随意、总量控制
void LineTextDataCreator::loadFromPattern(const std::string &filepath) {
    std::vector<SpliceableText> originSTVec, tempSTVec;
    std::unordered_set<SpliceableText, HashSpliceableText> originST, tempST;

    std::unordered_set<std::string> fruits;
    std::vector<std::string> c4, c3, c2, c1;
    c4 = {"C", "C", "C", "C"};
    c3 = {"N", "N", "N", "N", "P", "Al"};
    c2 = {"O", "S", "O", "S", "O", "S", "Mg", "Zn"};
    c1 = {"F", "Cl", "Br", "I", "H", "H", "H", "H"};
//    c4 = {"C"};
//    c3 = {"N"};
//    c2 = {"O", "S"};
//    c1 = {"H"};
    for (int i = 0; i < 1000; i++) {
        for (auto &str: c4) {
            originSTVec.emplace_back(str, 3, 1);
            originSTVec.emplace_back(str, 1, 3);
            originSTVec.emplace_back(str, 2, 2);
            originSTVec.emplace_back(str + StdUtil::randSelect(c1) + "3", 1, 0);//-CH3
            originSTVec.emplace_back(str + "(" + str + StdUtil::randSelect(c1) + "3)3", 1, 0);//-C(CH3)3
            originSTVec.emplace_back(str + "H(" + str + StdUtil::randSelect(c1) + "3)2", 1, 0);//-CH(CH3)2
            originSTVec.emplace_back(str + "(" + str + StdUtil::randSelect(c1) + "3)2", 2, 0);//=C(CH3)2
            originSTVec.emplace_back(str + StdUtil::randSelect(c1) + "3", 0, 1);//CH3-
            originSTVec.emplace_back(StdUtil::randSelect(c1) + "3" + str, 0, 1);//H3C-
            originSTVec.emplace_back(str + StdUtil::randSelect(c1) + "2", 2, 0);//=CH2
            originSTVec.emplace_back(StdUtil::randSelect(c1) + "2" + str, 0, 2);//H2C=
            originSTVec.emplace_back(str + StdUtil::randSelect(c1) + "2", 1, 1);//-CH2-
            originSTVec.emplace_back(str + StdUtil::randSelect(c1), 3, 0);// #CH
            originSTVec.emplace_back(StdUtil::randSelect(c1) + str, 0, 3);//HC#
            originSTVec.emplace_back(str + StdUtil::randSelect(c1), 2, 1);//=CH-
            originSTVec.emplace_back(str + StdUtil::randSelect(c1), 1, 2);//-CH=
        }
        for (auto &str: c3) {
            originSTVec.emplace_back(str, 3, 0);//#N
            originSTVec.emplace_back(str, 0, 3);//N#
            originSTVec.emplace_back(str + StdUtil::randSelect(c1) + "2", 1, 0);//-NH2
            originSTVec.emplace_back(StdUtil::randSelect(c1) + "2" + str, 0, 1);//H2N-
            originSTVec.emplace_back(str + StdUtil::randSelect(c1), 2, 0);//=NH
            originSTVec.emplace_back(StdUtil::randSelect(c1) + str, 0, 2);//HN=
            originSTVec.emplace_back(str + StdUtil::randSelect(c1), 1, 1);//-NH-
        }
        for (auto &str: c2) {
            originSTVec.emplace_back(str, 0, 2);//O=
            originSTVec.emplace_back(str, 2, 0);//=O
            originSTVec.emplace_back(str + StdUtil::randSelect(c1), 1, 0);//-OH
            originSTVec.emplace_back(StdUtil::randSelect(c1) + str, 0, 1);//HO-
        }
        for (auto &str: c1) {
            originSTVec.emplace_back(str, 1, 0);//H-
            originSTVec.emplace_back(str, 0, 1);//-H
        }
    }
    xgd::sout << "step1.size=" << originSTVec.size() << std::endl;
    QFile file(filepath.c_str());
    file.open(QIODevice::ReadOnly);
    QString str = file.readAll();
    file.close();
    QTextStream iss(&str);
//    std::istringstream iss(str);
    QString line;
    while (!iss.atEnd()) {
        line = iss.readLine();
        QString a, b;
        QTextStream iss_line(&line);
        try {
            iss_line >> a;
            if (!a.isEmpty() && '#' == a[0])continue;
            iss_line >> b;
//            xgd::sout << "\"" << b << "\",";
            if ('-' != a.front() && '-' != a.back()) {
                originSTVec.emplace_back(a.toStdString(), 1, 0);//CO2H
            }
            if ('-' != b.front() && '-' != b.back()) {
                originSTVec.emplace_back(b.toStdString(), 0, 1);//HO2C
            }
        } catch (std::exception &e) {
            xgd::serr << e.what() << std::endl;
        }
    }
    xgd::sout << "step2.size=" << originSTVec.size() << std::endl;
    size_t maxLoop = 49;
    for (int j = 0; j < 1000; j++) {
        for (size_t i = 2; i <= maxLoop; i++) {
            originSTVec.emplace_back(
                    StdUtil::randSelect(c4) + std::to_string(i) +
                    StdUtil::randSelect(c1) + std::to_string(2 * i), 1, 1);
            originSTVec.emplace_back(
                    StdUtil::randSelect(c3) + std::to_string(i) +
                    StdUtil::randSelect(c1) + std::to_string(i), 1, 1);
        }
    }
    xgd::sout << "step3.size=" << originSTVec.size() << std::endl;
    for (auto &st: originSTVec) {
        originST.insert(st);
    }
    originSTVec.clear();
    originSTVec.reserve(originST.size());
    for (auto &st: originST) {
        originSTVec.push_back(st);
    }
    xgd::sout << "step4.size=" << originSTVec.size() << std::endl;
//    for (auto &st:originSTVec) {
//        xgd::sout << st.getNeutral() << std::endl;
//    }
//    xgd::sout << "**************\n";
//    exit(-1);
    tempST = originST;
    tempSTVec = originSTVec;
    for (auto &st3: originSTVec) {
        auto str = st3.getRaw();
        if (str.length() <= 7) {// 且不存在
            fruits.insert(str);
            if (str.front() == '-') {
                auto str2 = str;
                str2.front() = '+';
                fruits.insert(str2);
                str2.front() = '_';
                fruits.insert(str2);
            }
            if (str.back() == '-') {
                auto str2 = str;
                str2.front() = '+';
                fruits.insert(str2);
                str2.front() = '_';
                fruits.insert(str2);
            }
        }
        str = st3.getNeutral();
        if (str.length() <= 7) {// 且不存在
            fruits.insert(str);
            if (str.front() == '-') {
                auto str2 = str;
                str2.front() = '+';
                fruits.insert(str2);
                str2.front() = '_';
                fruits.insert(str2);
            }
            if (str.back() == '-') {
                auto str2 = str;
                str2.front() = '+';
                fruits.insert(str2);
                str2.front() = '_';
                fruits.insert(str2);
            }
        }
    }
    while (fruits.size() < 10000) {
        auto st1 = StdUtil::randSelect(originSTVec);
        auto st2 = StdUtil::randSelect(tempSTVec);
        if (StdUtil::byProb(0.5)) {
            std::swap(st1, st2);
        }
        auto st3 = joinSpliceableText(st1, st2);
        if (st3 && !st3->isFull()) {// 满足新建条件
            auto str = st3->getNeutral();
            if (str.length() <= 7 && StdUtil::notExist(tempST, st3.value())) {// 且不存在
                tempST.insert(st3.value());
                tempSTVec.push_back(st3.value());
                fruits.insert(std::move(str));
            }
        }
    }
    for (auto &st: fruits) {
//        xgd::sout << st << std::endl;
        wordSet.insert(st);
    }
    updateCharSet();
}

const std::unordered_set<char> &LineTextDataCreator::getCharSet() const {
    return charSet;
}

const std::unordered_set<std::string> &LineTextDataCreator::getWordSet() const {
    return wordSet;
}
