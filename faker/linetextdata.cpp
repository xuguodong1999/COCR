#include "linetextdata.hpp"
#include "std_util.hpp"
#include <QFile>
#include <QJsonObject>
#include <QJsonParseError>
#include <sstream>
#include <vector>

extern std::vector<std::string> ElementsData;

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
    std::vector<std::string> fourthGroup = {
            "C", "Si", "Ge", "Sn", "Pb"
    }, thirdGroup = {
            "N", "P", "As", "Sb", "Bi", "Al"
    }, leftRight = {
            "O", "S", "Mg"
    }, i1 = {
            "F", "Cl", "Br", "I", "Na", "H", "K"
    }, left, right;
    std::string con2 = "=", con3 = "#", con1 = "-";
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
            right.push_back(a);//CO2H
            left.push_back(b);//HO2C
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    for(auto&ii1:i1){
        right.push_back(ii1);
        left.push_back(ii1);
    }
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

    i2.push_back("CH2");
    i2.push_back("CH2");
    std::vector<std::string> oneByOne = {"CH2", "NH", ""};
    size_t maxLoop = 999;
    for (size_t i = 2; i <= maxLoop; i++) {

        oneByOne.push_back(
                "C" + std::to_string(i) + "H" + std::to_string(2 * i));
        oneByOne.push_back(
                "N" + std::to_string(i) + "H" + std::to_string(i));
    }
}
