#include "hw_str.hpp"
#include "hw_data.hpp"
#include "std_util.hpp"
#include <iostream>


std::string HwStr::getShownText() const {
    if (richText.empty())return "[]";
    std::string result = "[";
    auto[charData, lastCharType]=richText[0];
    result += charData;
    for (size_t i = 1; i < richText.size(); ++i) {
        result += richText[i].first;
        if (richText[i].second != lastCharType) {
            lastCharType = richText[i].second;
            result += "_";
        }
    }
    result += "]";
    return std::move(result);
}

std::shared_ptr<HwItem> HwStr::GetSpecText(const HwSpecText &_specText) {
    auto specText = std::make_shared<HwStr>();
    auto &dataLoader = HwDataLoader::getInstance();
    switch (_specText) {
        case HwSpecText::PositiveElec: {
            auto sym = dataLoader.GetByStrLabel("⊕");
            specText->push_back(sym);
            break;
        }
        case HwSpecText::NegativeElec: {
            auto sym1 = dataLoader.GetShape(ShapeType::Circle);
            sym1.resizeTo(100, 100);
            sym1.moveCenterTo(cv::Point2f(50, 50));
            auto sym2 = dataLoader.GetByStrLabel("-");
            sym2.resizeTo(80, 80);
            sym2.moveCenterTo(cv::Point2f(50, 50));
            specText->push_back(sym1);
            specText->push_back(sym2);
            break;
        }
        case HwSpecText::TripleHorizontalLine: {
            float w = betweenProb(30, 80);
            auto sym1 = dataLoader.GetShape(ShapeType::Line);
            sym1.castToLine(cv::Point2f(0, 0), cv::Point2f(100, 0));
            auto sym2 = dataLoader.GetShape(ShapeType::Line);
            sym2.castToLine(cv::Point2f(0, w / 2), cv::Point2f(100, w / 2));
            auto sym3 = dataLoader.GetShape(ShapeType::Line);
            sym3.castToLine(cv::Point2f(0, w), cv::Point2f(100, w));
            specText->push_back(sym1);
            specText->push_back(sym2);
            specText->push_back(sym3);
            break;
        }
        default: {
            std::cerr << "HwSpecText:" << (int) _specText
                      << " not implemented in HwStr::GetSpecText" << std::endl;
            exit(-1);
        }
    }
    return specText;
}

DetectorClasses HwStr::getItemType() const {
    return label;
}

HwStr::HwStr() : label(DetectorClasses::ItemHorizontalStr) {

}


//centHeight = 32, refX = 0, refY = 0, refCentY = 16, refSubMinH = 12,randAngle=3;
void HwStr::pushCommon(const std::string &_text, const HwCharType &_hwCharType) {
    richText.emplace_back(_text, _hwCharType);
    auto &dataLoader = HwDataLoader::getInstance();
    auto sym = dataLoader.GetByStrLabel(_text);
    pushHwData(sym, _hwCharType);
}

void HwStr::pushSpecText(const HwSpecText &_specText, const HwCharType &_hwCharType) {
    auto sym = GetSpecText(_specText)->asScript();
    pushHwData(sym, _hwCharType);
}

void HwStr::pushBlank() {
    richText.emplace_back(" ", HwCharType::Normal);
    floatX += betweenProb(offset, centHeight);
}

HwCharType HwStr::decideHwCharType(const std::string &_text) const {
    auto it = sTextMap.find(_text);
    if (sTextMap.end() != it) {
        switch (it->second) {
            case HwSpecText::NegativeElec:
            case HwSpecText::PositiveElec: {
                return HwCharType::RightUp;
            }
            case HwSpecText::LineRightBracket:
            case HwSpecText::LineLeftBracket: {
                return HwCharType::Normal;
            }
            default: {
                return HwCharType::Normal;
            }
        }
    } else if (1 != _text.length()) {
        return HwCharType::Normal;
    } else {
        char c = _text[0];
        if ('0' <= c && c <= '9') {
            if (byProb(0.6))return HwCharType::RightDownSmall;
            else return HwCharType::RightDown;
        } else if ('a' <= c && c <= 'z') {
            if (byProb(0.6))return HwCharType::Normal;
            else return HwCharType::RightDown;
        } else {
            return HwCharType::Normal;
        }
    }
}

void HwStr::loadRichText(const std::vector<std::string> &_unlabeledText) {
    floatX = 0;
    for (auto &text:_unlabeledText) {
        auto it = sTextMap.find(text);
        if (sTextMap.end() == it) {
            pushCommon(text, decideHwCharType(text));
        } else {
            if (HwSpecText::Blank == it->second) {
                pushBlank();
            } else {
                auto tp = decideHwCharType(text);
                richText.emplace_back(text, tp);
                pushSpecText(it->second, tp);
            }
        }
    }
}

void HwStr::loadElement(const ElementType &_elementType) {
    loadPlainText(convertElementTypeToString(_elementType));
    // override label
    label = convertElementTypeToDetectorClasses(_elementType);
}

void HwStr::loadPlainText(const std::string &_plainText) {
    floatX = 0;
    label = DetectorClasses::ItemHorizontalStr;
    for (auto &character:_plainText) {
        pushCommon(std::string(1, character));
    }
}

HwStr::HwStr(const ElementType &_elementType) {
    loadElement(_elementType);
}

void HwStr::loadRichACSII(const std::string &_text) {
    std::vector<std::string> text;
    for (auto &character:_text) {
        text.push_back(std::string(1, character));
    }
    loadRichText(text);
}

void HwStr::pushHwData(HwScript &_hwScript, const HwCharType &_hwCharType) {
    _hwScript.rotate(betweenProb(-randAngle, randAngle));
    if (HwCharType::Normal == _hwCharType) {
        float w = betweenProb(centHeight, maxHeight);
        _hwScript.resizeTo(w, w, true);
        auto rect = _hwScript.getBoundingBox();
        float offsetY = std::min(offset, (maxHeight - rect->height) / 2);
        float centY = betweenProb(-offsetY, offsetY) + maxHeight / 2;
        float offsetX = betweenProb(0, offsetW) + rect->width / 2;
        _hwScript.moveCenterTo(cv::Point2f(floatX + offsetX, centY));
        floatX += (rect->width / 2 + offsetX);
    } else if (HwCharType::RightDownSmall == _hwCharType) {
        float w = betweenProb(subCentHeight, subMaxHeight);
        _hwScript.resizeTo(w, w, true);
        auto rect = _hwScript.getBoundingBox();
        float offsetY = ((1 - subStartMinRatio) * maxHeight - rect->height) / 2;
        float centY = betweenProb(-offsetY, offsetY) + maxHeight * (subStartMinRatio + 1) / 2;
        float offsetX = betweenProb(0, subOffsetW) + rect->width / 2;
        _hwScript.moveCenterTo(cv::Point2f(floatX + offsetX, centY));
        floatX += (rect->width + offsetX);
    } else if (HwCharType::RightDown == _hwCharType) {
        float w = betweenProb(subCentHeight2, subMaxHeight2);
        _hwScript.resizeTo(w, w, true);
        auto rect = _hwScript.getBoundingBox();
        float offsetY = ((1 - subStartMinRatio2) * maxHeight - rect->height) / 2;
        float centY = betweenProb(-offsetY, offsetY) + maxHeight * (subStartMinRatio2 + 1) / 2;
        float offsetX = betweenProb(0, subOffsetW2) + rect->width / 2;
        _hwScript.moveCenterTo(cv::Point2f(floatX + offsetX, centY));
        floatX += (rect->width + offsetX);
    } else if (HwCharType::RightUp == _hwCharType) {
        float w = betweenProb(subCentHeight, subMaxHeight);
        _hwScript.resizeTo(w, w, true);
        auto rect = _hwScript.getBoundingBox();
        float offsetY = ((1 - subStartMinRatio) * maxHeight - rect->height) / 2;
        float centY = betweenProb(-offsetY, offsetY) + maxHeight * (subStartMinRatio + 1) / 2;
        centY = maxHeight - centY;
        float offsetX = betweenProb(0, subOffsetW) + rect->width / 2;
        _hwScript.moveCenterTo(cv::Point2f(floatX + offsetX, centY));
        floatX += (rect->width + offsetX);
    } else {
        std::cerr << "unhandled hwCharType: " << (int) _hwCharType << std::endl;
        exit(-1);
    }
    push_back(_hwScript);
}

