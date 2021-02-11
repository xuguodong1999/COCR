#include "linetextdata.hpp"
#include "std_util.hpp"
#include <QFile>
#include <QJsonObject>
#include <QJsonParseError>
#include <sstream>
#include <vector>
#include <optional>

extern std::vector<std::string> ElementsData;

SpliceableText::SpliceableText() : l(0), r(0) {}

SpliceableText::SpliceableText(std::string _text, int _l, int _r)
        : text(std::move(_text)), l(_l), r(_r) {}


const std::string &SpliceableText::getRaw() const {
    return text;
}

std::string SpliceableText::getRand() const {
    if (l && r) {
        return cons[rand() % std::min(l, 3)] + text + cons[rand() % std::min(r, 3)];
    } else if (l) {
        return cons[rand() % std::min(l, 3)] + text;
    } else if (r) {
        // 右侧有自由基
        return text + cons[rand() % std::min(r, 3)];
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
        std::cerr << "i=" << i << std::endl;
        exit(-1);
    }
    auto st = _st;
    st.r = i;
    if (i == 1 && byProb(0.5)) {
        // ignore -
    } else {
        st.text = st.text + SpliceableText::cons[i];
    }
    return st;
}

SpliceableText operator+(int i, const SpliceableText &_st) {
    if (i < 0 || i > 3) {
        std::cerr << "i=" << i << std::endl;
        exit(-1);
    }
    auto st = _st;
    st.l = i;
    if (i == 1 && byProb(0.5)) {
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


void LineTextDataCreator::loadFromSuperAtom(const char *_filepath) {
    QFile file(_filepath);
    file.open(QIODevice::ReadOnly);
    auto str = file.readAll().toStdString();
    file.close();
    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line)) {
        std::string a, b;
        std::istringstream iss_line(line);
        try {
            iss_line >> a;
            if (!a.empty() && '#' == a[0])continue;
            iss_line >> b;
            wordSet.insert(a);
            wordSet.insert(b);
            wordSet.insert("-" + a);
            wordSet.insert(b + "-");
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    updateCharSet();
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

    for (auto &key:doc.object().keys()) {
        auto text = key.simplified();
        wordSet.insert(text.toLower().toStdString());
        wordSet.insert(text.toUpper().toStdString());
    }
    updateCharSet();
}

void LineTextDataCreator::clearSet() {
    charSet.clear();
    wordSet.clear();
}

void LineTextDataCreator::loopOnce(const std::function<void(const std::string &)> &_func) {
    for (auto &word:wordSet) {
        _func(word);
    }
}

void LineTextDataCreator::displaySet() {
    std::cout << "charSet.size=" << charSet.size() << "\n";
    for (auto &c:charSet) {
        std::cout << c << ", ";
    }
    std::cout << std::endl;
    std::cout << "wordSet.size=" << wordSet.size() << "\n";
//    for (auto &word:wordSet) {
//        std::cout << word << ", ";
//    }
//    std::cout << std::endl;
}

void LineTextDataCreator::updateCharSet() {
    charSet.clear();
    for (auto &word:wordSet) {
        for (auto &c:word) {
            charSet.insert(c);
        }
    }
}


// 规则限定、连接随意、总量控制
void LineTextDataCreator::loadFromPattern(const char *_filepath) {
    std::vector<SpliceableText> originSTVec, tempSTVec;
    std::unordered_set<SpliceableText, HashSpliceableText> originST, tempST;

    std::unordered_set<std::string> fruits;
    std::vector<std::string> c4, c3, c2, c1;
    c4 = {"C", "C", "C", "C", "Si", "Ge", "Sn", "Pb"};
    c3 = {"N", "N", "N", "N", "P", "As", "Sb", "Bi", "Al"};
    c2 = {"O", "S", "O", "S", "O", "S", "Mg", "Zn"};
    c1 = {"F", "Cl", "Br", "I", "H", "H", "H", "H"};
//    c4 = {"C"};
//    c3 = {"N"};
//    c2 = {"O", "S"};
//    c1 = {"H"};
    for (int i = 0; i < 10; i++) {
        for (auto &str:c4) {
            originSTVec.emplace_back(str, 3, 1);
            originSTVec.emplace_back(str, 1, 3);
            originSTVec.emplace_back(str, 2, 2);
            originSTVec.emplace_back(str + randSelect(c1) + "3", 1, 0);//-CH3
            originSTVec.emplace_back(str + randSelect(c1) + "3", 0, 1);//CH3-
            originSTVec.emplace_back(randSelect(c1) + "3" + str, 0, 1);//H3C-
            originSTVec.emplace_back(str + randSelect(c1) + "2", 2, 0);//=CH2
            originSTVec.emplace_back(randSelect(c1) + "2" + str, 0, 2);//H2C=
            originSTVec.emplace_back(str + randSelect(c1) + "2", 1, 1);//-CH2-
            originSTVec.emplace_back(str + randSelect(c1), 3, 0);// #CH
            originSTVec.emplace_back(randSelect(c1) + str, 0, 3);//HC#
            originSTVec.emplace_back(str + randSelect(c1), 2, 1);//=CH-
            originSTVec.emplace_back(str + randSelect(c1), 1, 2);//-CH=
        }
        for (auto &str:c3) {
            originSTVec.emplace_back(str, 3, 0);//#N
            originSTVec.emplace_back(str, 0, 3);//N#
            originSTVec.emplace_back(str + randSelect(c1) + "2", 1, 0);//-NH2
            originSTVec.emplace_back(randSelect(c1) + "2" + str, 0, 1);//H2N-
            originSTVec.emplace_back(str + randSelect(c1), 2, 0);//=NH
            originSTVec.emplace_back(randSelect(c1) + str, 0, 2);//HN=
            originSTVec.emplace_back(str + randSelect(c1), 1, 1);//-NH-
        }
        for (auto &str:c2) {
            originSTVec.emplace_back(str, 0, 2);//O=
            originSTVec.emplace_back(str, 2, 0);//=O
            originSTVec.emplace_back(str + randSelect(c1), 1, 0);//-OH
            originSTVec.emplace_back(randSelect(c1) + str, 0, 1);//HO-
        }
        for (auto &str:c1) {
            originSTVec.emplace_back(str, 1, 0);//H-
            originSTVec.emplace_back(str, 0, 1);//-H
        }
    }
    std::cout << "step1.size=" << originSTVec.size() << std::endl;
    QFile file(_filepath);
    file.open(QIODevice::ReadOnly);
    auto str = file.readAll().toStdString();
    file.close();
    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line)) {
        std::string a, b;
        std::istringstream iss_line(line);
        try {
            iss_line >> a;
            if (!a.empty() && '#' == a[0])continue;
//            iss_line >> b;
//            if ('-' != a.front() && '-' != a.back())
//                originSTVec.emplace_back(a, 1, 0);//CO2H
//            if ('-' != b.front() && '-' != b.back())
//                originSTVec.emplace_back(b, 0, 1);//HO2C
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    std::cout << "step2.size=" << originSTVec.size() << std::endl;
    size_t maxLoop = 4;
    for (size_t i = 2; i <= maxLoop; i++) {
        originSTVec.emplace_back(
                randSelect(c4) + std::to_string(i) +
                randSelect(c1) + std::to_string(2 * i), 1, 1);
        originSTVec.emplace_back(
                randSelect(c3) + std::to_string(i) +
                randSelect(c1) + std::to_string(i), 1, 1);
    }
    std::cout << "step3.size=" << originSTVec.size() << std::endl;
    for (auto &st:originSTVec) {
        originST.insert(st);
    }
    originSTVec.clear();
    originSTVec.reserve(originST.size());
    for (auto &st:originST) {
        originSTVec.push_back(std::move(st));
    }
    std::cout << "step4.size=" << originSTVec.size() << std::endl;
//    for (auto &st:originSTVec) {
//        std::cout << st.getNeutral() << std::endl;
//    }
//    std::cout << "**************\n";
//    exit(-1);
    tempST = originST;
    tempSTVec = originSTVec;
    while (fruits.size() < 5000) {
        auto st1 = randSelect(originSTVec);
        auto st2 = randSelect(tempSTVec);
        if (byProb(0.5))std::swap(st1, st2);
        auto st3 = joinSpliceableText(st1, st2);
        if (st3 && !st3->isFull()) {// 满足新建条件
            auto str = st3->getNeutral();
            if (str.length() < 10 && notExist(tempST, st3.value())) {// 且不存在
                tempST.insert(st3.value());
                tempSTVec.push_back(st3.value());
                std::cout << str << std::endl;
                fruits.insert(std::move(str));
            }
        }
    }
//    for (auto &st:fruits) {
//        std::cout << st << std::endl;
//    }
}
